//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_POLYGONGRAPHICSITEM_H
#define MARBLE_POLYGONGRAPHICSITEM_H

#include "ScreenGraphicsItem.h"
#include "marble_export.h"

#include <QPolygonF>

#include "GeoDataCoordinates.h"

namespace Marble
{

class PolygonGraphicsItemPrivate;

/**
 * @brief A graphicsitem representing a polygon.
 */
class MARBLE_EXPORT PolygonGraphicsItem : public ScreenGraphicsItem
{
  public:
    explicit PolygonGraphicsItem( MarbleGraphicsItem *parent = 0 );
    ~PolygonGraphicsItem();

    QPolygonF polygon() const;
    void setPolygon( const QPolygonF &polygon );

    QBrush brush() const;
    void setBrush( const QBrush &brush );

    QPen pen() const;
    void setPen( const QPen &pen );

    void resetTransformation();
    void rotate( qreal angle );
    void translate( qreal dx, qreal dy );
    void scale (qreal sx, qreal sy );

  protected:
    void paint( QPainter *painter ) const;

  private:
    PolygonGraphicsItemPrivate * const d;
};

} // Marble namespace

#endif
