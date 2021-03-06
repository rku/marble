//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_GEOPHOTOGRAPHICSITEM_H
#define MARBLE_GEOPHOTOGRAPHICSITEM_H

#include "GeoDataPoint.h"
#include "GeoGraphicsItem.h"
#include "marble_export.h"

#include <QtCore/QRectF>
#include <QtGui/QImage>

namespace Marble
{

class MARBLE_EXPORT GeoPhotoGraphicsItem : public GeoGraphicsItem
{
public:
    explicit GeoPhotoGraphicsItem( const GeoDataFeature *feature );

    void setPoint( const GeoDataPoint& point );

    GeoDataPoint point() const;

    void setPhoto( const QImage &photo );

    QImage photo() const;

    void setPhotoFile( const QString &filename );

    QString photoPath() const;

    void setViewport( const ViewportParams *viewport );

    void paint( GeoPainter* painter ) const;

    virtual const GeoDataLatLonAltBox& latLonAltBox() const;

protected:
    GeoDataPoint m_point;

    mutable QImage m_photo;

    QString m_photoPath;

    QRectF m_positionRect;
};

}

#endif
