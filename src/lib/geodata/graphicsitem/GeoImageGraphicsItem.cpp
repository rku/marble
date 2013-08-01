//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "GeoImageGraphicsItem.h"

#include "GeoGraphicsItem_p.h"
#include "GeoPainter.h"
#include "ViewportParams.h"

namespace Marble
{

GeoImageGraphicsItem::GeoImageGraphicsItem( const GeoDataFeature *feature )
    :GeoGraphicsItem( feature )
{
}

void GeoImageGraphicsItem::setLatLonBox( const GeoDataLatLonBox &box )
{
    p()->m_latLonAltBox = GeoDataLatLonAltBox( box, 0, 0 );
}

const GeoDataLatLonBox& GeoImageGraphicsItem::latLonBox() const
{
    return p()->m_latLonAltBox;
}

void GeoImageGraphicsItem::setImage( const QImage &image )
{
    m_image = image;
}

QImage GeoImageGraphicsItem::image() const
{
    return m_image;
}

void GeoImageGraphicsItem::setImageFile( const QString &filename )
{
    m_imageFile = filename;
}

QString GeoImageGraphicsItem::imageFile() const
{
    return m_imageFile;
}

void GeoImageGraphicsItem::setViewport( const ViewportParams *viewport )
{
    bool unloadImage = true;
    if ( viewport->projection() != Spherical ) {
        qreal x1(0.0), x2(0.0), y1( 0.0 ), y2( 0.0 );
        viewport->screenCoordinates( p()->m_latLonAltBox.west(), p()->m_latLonAltBox.north(), x1, y1 );
        viewport->screenCoordinates( p()->m_latLonAltBox.east(), p()->m_latLonAltBox.south(), x2, y2 );
        QRectF const screen( QPointF( 0, 0), viewport->size() );
        QRectF const position( x1, y1, x2-x1, y2-y1 );
        if ( !(screen & position).isEmpty() ) {
            if ( m_image.isNull() && !m_imageFile.isEmpty() ) {
                /** @todo: Load in a thread */
                m_image = QImage( m_imageFile );
                m_position = position;
            }
            unloadImage = false;
        }
    } else {
        /** @todo: Implement for spherical projection, possibly reusing code
                   from SphericalScanlineTextureMapper */
    }

    if ( unloadImage && !m_imageFile.isEmpty() ) {
        // No unloading if no path is known
        m_image = QImage();
        m_position = QRectF();
    }
}

void GeoImageGraphicsItem::paint( GeoPainter* painter ) const
{
    if ( m_image.isNull() || !m_position.isValid() )
        return;

    /** @todo: Respect x-repeat */

    painter->drawImage( m_position, m_image );
}

}
