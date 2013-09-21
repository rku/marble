//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Rene Kuettner <rene@bitkanal.net>
//

// Self
#include "PolygonGraphicsItem.h"
#include "PolygonGraphicsItem_p.h"

// Marble

// Qt
#include <QPainter>

namespace Marble {

PolygonGraphicsItem::PolygonGraphicsItem( MarbleGraphicsItem *parent )
        : ScreenGraphicsItem( parent ),
          d( new PolygonGraphicsItemPrivate( this ) )
{
}

PolygonGraphicsItem::~PolygonGraphicsItem()
{
    delete d;
}

QPolygonF PolygonGraphicsItem::polygon() const
{
    return d->m_polygon;
}

void PolygonGraphicsItem::setPolygon( const QPolygonF &polygon )
{
    d->m_polygon = polygon;
}

QBrush PolygonGraphicsItem::brush() const
{
    return d->m_brush;
}

void PolygonGraphicsItem::setBrush( const QBrush &brush )
{
    d->m_brush = brush;
}

QPen PolygonGraphicsItem::pen() const
{
    return d->m_pen;
}

void PolygonGraphicsItem::setPen( const QPen &pen )
{
    d->m_pen = pen;
}

void PolygonGraphicsItem::resetTransformation()
{
    d->m_transform.reset();
}

void PolygonGraphicsItem::rotate( qreal angle )
{
    d->m_transform.rotate( angle );
}

void PolygonGraphicsItem::translate( qreal dx, qreal dy )
{
    d->m_transform.translate( dx, dy );
}

void PolygonGraphicsItem::scale (qreal sx, qreal sy )
{
    d->m_transform.scale( sx, sy );
}

void PolygonGraphicsItem::paint( QPainter *painter ) const
{
    painter->save();

    if ( !d->m_transform.isIdentity() ) {
        painter->setTransform( d->m_transform );
    }
    painter->setBrush( d->m_brush );
    painter->setPen( d->m_pen );
    painter->drawPolygon( d->m_polygon );

    painter->restore();
}

} // namespace Marble
