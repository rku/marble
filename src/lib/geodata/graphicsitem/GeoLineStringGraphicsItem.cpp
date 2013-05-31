//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "GeoLineStringGraphicsItem.h"

#include "GeoDataLineString.h"
#include "GeoDataLineStyle.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "GeoDataStyle.h"

namespace Marble
{

GeoLineStringGraphicsItem::GeoLineStringGraphicsItem( const GeoDataFeature *feature, const GeoDataLineString* lineString )
        : GeoGraphicsItem( feature ),
          m_lineString( lineString ),
          m_penWidth( 1 )
{
}

void GeoLineStringGraphicsItem::setLineString( const GeoDataLineString* lineString )
{
    m_lineString = lineString;
}

const GeoDataLatLonAltBox& GeoLineStringGraphicsItem::latLonAltBox() const
{
    return m_lineString->latLonAltBox();
}

void GeoLineStringGraphicsItem::setViewport( const ViewportParams *viewport )
{
    if ( style() ) {
        if ( style()->lineStyle().physicalWidth() != 0.0 ) {
            if ( float( viewport->radius() ) / EARTH_RADIUS * style()->lineStyle().physicalWidth() < style()->lineStyle().width() )
                m_penWidth = style()->lineStyle().width();
            else
                m_penWidth = float( viewport->radius() ) / EARTH_RADIUS * style()->lineStyle().physicalWidth();
        }
    }
}

void GeoLineStringGraphicsItem::paint( GeoPainter* painter ) const
{
    LabelPositionFlags label_position_flags = NoLabel;

    painter->save();

    if ( !style() ) {
        painter->setPen( QPen() );
    }
    else {
        QPen currentPen = painter->pen();

        if ( currentPen.color() != style()->lineStyle().paintedColor() )
            currentPen.setColor( style()->lineStyle().paintedColor() );

        currentPen.setWidthF( m_penWidth );

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
            QBrush brush = painter->background();
            brush.setColor( style()->polyStyle().paintedColor() );
            painter->setBackground( brush );

            painter->setBackgroundMode( Qt::OpaqueMode );
        }

        // label styles
        painter->setFont( style()->labelStyle().font() );
        if ( style()->labelStyle().alignment() == GeoDataLabelStyle::Corner )
            label_position_flags |= LineStart;
        if ( style()->labelStyle().alignment() == GeoDataLabelStyle::Center )
            label_position_flags |= LineCenter;
    }

    painter->drawPolyline( *m_lineString, feature()->name(), label_position_flags );

    painter->restore();
}

}
