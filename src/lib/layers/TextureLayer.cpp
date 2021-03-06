//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008, 2009, 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
// Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>//

#include "TextureLayer.h"

#include <QtCore/qmath.h>
#include <QtCore/QTimer>

#include "SphericalScanlineTextureMapper.h"
#include "EquirectScanlineTextureMapper.h"
#include "MercatorScanlineTextureMapper.h"
#include "TileScalingTextureMapper.h"
#include "GeoPainter.h"
#include "GeoSceneGroup.h"
#include "GeoSceneTypes.h"
#include "MergedLayerDecorator.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "StackedTile.h"
#include "StackedTileLoader.h"
#include "SunLocator.h"
#include "TextureColorizer.h"
#include "TileLoader.h"
#include "ViewportParams.h"

namespace Marble
{

const int REPAINT_SCHEDULING_INTERVAL = 1000;

class TextureLayer::Private
{
public:
    Private( HttpDownloadManager *downloadManager,
             const SunLocator *sunLocator,
             const PluginManager *pluginManager,
             TextureLayer *parent );

    void requestDelayedRepaint();
    void updateTextureLayers();
    void updateTile( const TileId &tileId, const QImage &tileImage );

public:
    TextureLayer  *const m_parent;
    const SunLocator *const m_sunLocator;
    TileLoader m_loader;
    MergedLayerDecorator m_layerDecorator;
    StackedTileLoader    m_tileLoader;
    int m_tileZoomLevel;
    TextureColorizer *m_texcolorizer;
    QVector<const GeoSceneTextureTile *> m_textures;
    const GeoSceneGroup *m_textureLayerSettings;
    QString m_runtimeTrace;
    // For scheduling repaints
    QTimer           m_repaintTimer;

};

TextureLayer::Private::Private( HttpDownloadManager *downloadManager,
                                const SunLocator *sunLocator,
                                const PluginManager *pluginManager,
                                TextureLayer *parent )
    : m_parent( parent )
    , m_sunLocator( sunLocator )
    , m_loader( downloadManager, pluginManager )
    , m_layerDecorator( &m_loader, sunLocator )
    , m_tileLoader( &m_layerDecorator )
    , m_tileZoomLevel( -1 )
    , m_texcolorizer( 0 )
    , m_textureLayerSettings( 0 )
    , m_repaintTimer()
{
}

void TextureLayer::Private::requestDelayedRepaint()
{
    if ( !m_repaintTimer.isActive() ) {
        m_repaintTimer.start();
    }
}

void TextureLayer::Private::updateTextureLayers()
{
    QVector<GeoSceneTextureTile const *> result;

    foreach ( const GeoSceneTextureTile *candidate, m_textures ) {
        bool enabled = true;
        if ( m_textureLayerSettings ) {
            const bool propertyExists = m_textureLayerSettings->propertyValue( candidate->name(), enabled );
            enabled |= !propertyExists; // if property doesn't exist, enable texture nevertheless
        }
        if ( enabled ) {
            result.append( candidate );
            mDebug() << "enabling texture" << candidate->name();
        } else {
            mDebug() << "disabling texture" << candidate->name();
        }
    }

    m_layerDecorator.setTextureLayers( result );
    m_tileLoader.clear();

    emit m_parent->repaintNeeded();
}

void TextureLayer::Private::updateTile( const TileId &tileId, const QImage &tileImage )
{
    if ( tileImage.isNull() )
        return; // keep tiles in cache to improve performance

    m_tileLoader.updateTile( tileId, tileImage );

    requestDelayedRepaint();
}



TextureLayer::TextureLayer( HttpDownloadManager *downloadManager,
                            const SunLocator *sunLocator,
                            const PluginManager *pluginManager )
    : QObject()
    , d( new Private( downloadManager, sunLocator, pluginManager, this ) )
{
    connect( &d->m_loader, SIGNAL(tileCompleted(TileId,QImage)),
             this, SLOT(updateTile(TileId,QImage)) );

    // Repaint timer
    d->m_repaintTimer.setSingleShot( true );
    d->m_repaintTimer.setInterval( REPAINT_SCHEDULING_INTERVAL );
    connect( &d->m_repaintTimer, SIGNAL(timeout()),
             this, SIGNAL(repaintNeeded()) );
}

TextureLayer::~TextureLayer()
{
    delete d->m_texcolorizer;
    delete d;
}

QStringList TextureLayer::renderPosition() const
{
    return QStringList() << "SURFACE";
}

void TextureLayer::addSeaDocument( const GeoDataDocument *seaDocument )
{
    if( d->m_texcolorizer ) {
        d->m_texcolorizer->addSeaDocument( seaDocument );
        reset();
    }
}

void TextureLayer::addLandDocument( const GeoDataDocument *landDocument )
{
    if( d->m_texcolorizer ) {
        d->m_texcolorizer->addLandDocument( landDocument );
        reset();
    }
}

bool TextureLayer::showSunShading() const
{
    return d->m_layerDecorator.showSunShading();
}

bool TextureLayer::showCityLights() const
{
    return d->m_layerDecorator.showCityLights();
}

bool TextureLayer::setViewport( const ViewportParams *viewport )
{
    if ( d->m_layerDecorator.textureLayersSize() == 0 )
        return false;

    // choose the smaller dimension for selecting the tile level, leading to higher-resolution results
    const int levelZeroWidth = d->m_layerDecorator.tileSize().width() * d->m_layerDecorator.tileColumnCount( 0 );
    const int levelZeroHight = d->m_layerDecorator.tileSize().height() * d->m_layerDecorator.tileRowCount( 0 );
    const int levelZeroMinDimension = qMin( levelZeroWidth, levelZeroHight );

    // limit to 1 as dirty fix for invalid entry linearLevel
    const qreal linearLevel = qMax( 1.0, viewport->radius() * 4.0 / levelZeroMinDimension );

    // As our tile resolution doubles with each level we calculate
    // the tile level from tilesize and the globe radius via log(2)
    const qreal tileLevelF = qLn( linearLevel ) / qLn( 2.0 ) * 1.00001;  // snap to the sharper tile level a tiny bit earlier
                                                                         // to work around rounding errors when the radius
                                                                         // roughly equals the global texture width

    const int tileLevel = qMin<int>( d->m_layerDecorator.maximumTileLevel(), tileLevelF );

    if ( tileLevel != d->m_tileZoomLevel ) {
        d->m_tileZoomLevel = tileLevel;
        emit tileLevelChanged( d->m_tileZoomLevel );
    }

    d->m_runtimeTrace = QString("Cache: %1 ").arg(d->m_tileLoader.tileCount());
    return true;
}

bool TextureLayer::render( GeoPainter *painter, const QSize &viewportSize ) const
{
    // Stop repaint timer if it is already running
    d->m_repaintTimer.stop();

    if ( d->m_layerDecorator.textureLayersSize() == 0 )
        return false;

    const QRect rect( QPoint( 0, 0 ), viewportSize );
    painter->mapTexture( &d->m_tileLoader, d->m_tileZoomLevel, rect, d->m_texcolorizer );

    return true;
}

QString TextureLayer::runtimeTrace() const
{
    return d->m_runtimeTrace;
}

void TextureLayer::setShowRelief( bool show )
{
    if ( d->m_texcolorizer ) {
        d->m_texcolorizer->setShowRelief( show );
    }
}

void TextureLayer::setShowSunShading( bool show )
{
    disconnect( d->m_sunLocator, SIGNAL(positionChanged(qreal,qreal)),
                this, SLOT(reset()) );

    if ( show ) {
        connect( d->m_sunLocator, SIGNAL(positionChanged(qreal,qreal)),
                 this,       SLOT(reset()) );
    }

    d->m_layerDecorator.setShowSunShading( show );

    reset();
}

void TextureLayer::setShowCityLights( bool show )
{
    d->m_layerDecorator.setShowCityLights( show );

    reset();
}

void TextureLayer::setShowTileId( bool show )
{
    d->m_layerDecorator.setShowTileId( show );

    reset();
}

void TextureLayer::setVolatileCacheLimit( quint64 kilobytes )
{
    d->m_tileLoader.setVolatileCacheLimit( kilobytes );
}

void TextureLayer::reset()
{
    mDebug() << Q_FUNC_INFO;

    d->m_tileLoader.clear();
    emit repaintNeeded();
}

void TextureLayer::reload()
{
    foreach ( const TileId &id, d->m_tileLoader.visibleTiles() ) {
        // it's debatable here, whether DownloadBulk or DownloadBrowse should be used
        // but since "reload" or "refresh" seems to be a common action of a browser and it
        // allows for more connections (in our model), use "DownloadBrowse"
        d->m_layerDecorator.downloadStackedTile( id, DownloadBrowse );
    }
}

void TextureLayer::downloadStackedTile( const TileId &stackedTileId )
{
    d->m_layerDecorator.downloadStackedTile( stackedTileId, DownloadBulk );
}

void TextureLayer::setMapTheme( const QVector<const GeoSceneTextureTile *> &textures, const GeoSceneGroup *textureLayerSettings, const QString &seaFile, const QString &landFile )
{
    delete d->m_texcolorizer;
    d->m_texcolorizer = 0;

    if ( QFileInfo( seaFile ).isReadable() || QFileInfo( landFile ).isReadable() ) {
        d->m_texcolorizer = new TextureColorizer( seaFile, landFile );
    }

    d->m_textures = textures;
    d->m_textureLayerSettings = textureLayerSettings;

    if ( d->m_textureLayerSettings ) {
        connect( d->m_textureLayerSettings, SIGNAL(valueChanged(QString,bool)),
                 this,                      SLOT(updateTextureLayers()) );
    }

    d->updateTextureLayers();
}

int TextureLayer::tileZoomLevel() const
{
    return d->m_tileZoomLevel;
}

QSize TextureLayer::tileSize() const
{
    return d->m_layerDecorator.tileSize();
}

GeoSceneTiled::Projection TextureLayer::tileProjection() const
{
    return d->m_layerDecorator.tileProjection();
}

int TextureLayer::tileColumnCount( int level ) const
{
    return d->m_layerDecorator.tileColumnCount( level );
}

int TextureLayer::tileRowCount( int level ) const
{
    return d->m_layerDecorator.tileRowCount( level );
}

qint64 TextureLayer::volatileCacheLimit() const
{
    return d->m_tileLoader.volatileCacheLimit();
}

int TextureLayer::preferredRadiusCeil( int radius ) const
{
    const int tileWidth = d->m_layerDecorator.tileSize().width();
    const int levelZeroColumns = d->m_layerDecorator.tileColumnCount( 0 );
    const qreal linearLevel = 4.0 * (qreal)( radius ) / (qreal)( tileWidth * levelZeroColumns );
    const qreal tileLevelF = qLn( linearLevel ) / qLn( 2.0 );
    const int tileLevel = qCeil( tileLevelF );

    if ( tileLevel < 0 )
        return ( tileWidth * levelZeroColumns / 4 ) >> (-tileLevel);

    return ( tileWidth * levelZeroColumns / 4 ) << tileLevel;
}

int TextureLayer::preferredRadiusFloor( int radius ) const
{
    const int tileWidth = d->m_layerDecorator.tileSize().width();
    const int levelZeroColumns = d->m_layerDecorator.tileColumnCount( 0 );
    const qreal linearLevel = 4.0 * (qreal)( radius ) / (qreal)( tileWidth * levelZeroColumns );
    const qreal tileLevelF = qLn( linearLevel ) / qLn( 2.0 );
    const int tileLevel = qFloor( tileLevelF );

    if ( tileLevel < 0 )
        return ( tileWidth * levelZeroColumns / 4 ) >> (-tileLevel);

    return ( tileWidth * levelZeroColumns / 4 ) << tileLevel;
}

}

#include "TextureLayer.moc"
