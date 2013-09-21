//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_POLYGONGRAPHICSITEMPRIVATE_H
#define MARBLE_POLYGONGRAPHICSITEMPRIVATE_H

#include "ScreenGraphicsItem.h"
#include "marble_export.h"

#include <QPolygonF>
#include <QBrush>
#include <QPen>
#include <QBrush>
#include <QTransform>

namespace Marble
{

class MARBLE_EXPORT PolygonGraphicsItemPrivate
{
  public:
    explicit PolygonGraphicsItemPrivate( PolygonGraphicsItem *parent )
        : m_parent( parent )
    {
    }

    QPolygonF   m_polygon;
    QBrush      m_brush;
    QPen        m_pen;
    QTransform  m_transform;

    PolygonGraphicsItem * const m_parent;
};

} // Marble namespace

#endif
