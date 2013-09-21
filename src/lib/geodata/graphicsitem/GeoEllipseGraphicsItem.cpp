//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Rene Kuettner <rene@bitkanal.net>
//

#include "GeoEllipseGraphicsItem.h"

#include "GeoDataLineStyle.h"
#include "GeoDataCoordinates.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "GeoDataStyle.h"

namespace Marble
{

GeoEllipseGraphicsItem::GeoEllipseGraphicsItem( const GeoDataFeature *feature,
                                                const GeoDataCoordinates &center,
                                                qreal width, qreal height )
        : GeoGraphicsItem( feature ),
          m_latLonAltBox( center ),
          m_center( center ),
          m_width( width ),
          m_height( height ),
          m_isGeoProjected( false )
{
}

const GeoDataLatLonAltBox& GeoEllipseGraphicsItem::latLonAltBox() const
{
    // FIXME update latlonaltbox
    return m_latLonAltBox;
}

void GeoEllipseGraphicsItem::setCenter( const GeoDataCoordinates &center )
{
    m_center = center;
}

const GeoDataCoordinates GeoEllipseGraphicsItem::center() const
{
    return m_center;
}

void GeoEllipseGraphicsItem::setWidth( qreal width )
{
    m_width = width;
}

qreal GeoEllipseGraphicsItem::width() const
{
    return m_width;
}

void GeoEllipseGraphicsItem::setHeight( qreal height )
{
    m_height = height;
}

qreal GeoEllipseGraphicsItem::height() const
{
    return m_height;
}

void GeoEllipseGraphicsItem::setIsGeoProjected( bool isGeoProjected )
{
    m_isGeoProjected = isGeoProjected;
}

bool GeoEllipseGraphicsItem::isGeoProjected() const
{
    return m_isGeoProjected;
}

void GeoEllipseGraphicsItem::setViewport( const ViewportParams *viewport )
{
    Q_UNUSED( viewport );
}

void GeoEllipseGraphicsItem::paint( GeoPainter* painter ) const
{
    painter->save();

    if ( !style() ) {
        painter->setPen( QPen() );
    }
    else {
        QPen currentPen = painter->pen();

        if ( currentPen.color() != style()->lineStyle().paintedColor() )
            currentPen.setColor( style()->lineStyle().paintedColor() );

        currentPen.setWidthF( style()->lineStyle().width() );

        if ( currentPen.capStyle() != style()->lineStyle().capStyle() )
            currentPen.setCapStyle( style()->lineStyle().capStyle() );

        if ( currentPen.style() != style()->lineStyle().penStyle() )
            currentPen.setStyle( style()->lineStyle().penStyle() );

        if ( style()->lineStyle().penStyle() == Qt::CustomDashLine )
            currentPen.setDashPattern( style()->lineStyle().dashPattern() );

        if ( painter->mapQuality() != Marble::HighQuality
                && painter->mapQuality() != Marble::PrintQuality ) {
            QColor penColor = currentPen.color();
            penColor.setAlpha( 255 );
            currentPen.setColor( penColor );
        }

        if ( painter->pen() != currentPen )
            painter->setPen( currentPen );

        if ( style()->lineStyle().background() ) {
            painter->setBrush( style()->lineStyle().paintedColor() );
        }
    }

    painter->drawEllipse( m_center, m_width, m_height, m_isGeoProjected );

    painter->restore();
}

}
