//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_GEOELLIPSEGRAPHICSITEM_H
#define MARBLE_GEOELLIPSEGRAPHICSITEM_H

#include "GeoGraphicsItem.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataCoordinates.h"
#include "marble_export.h"

namespace Marble
{

class GeoDataLineStyle;

class MARBLE_EXPORT GeoEllipseGraphicsItem : public GeoGraphicsItem
{
public:
    explicit GeoEllipseGraphicsItem( const GeoDataFeature *feature,
                                     const GeoDataCoordinates &center,
                                     const qreal width, const qreal height );

    virtual const GeoDataLatLonAltBox& latLonAltBox() const;

    void setCenter( const GeoDataCoordinates &center );

    const GeoDataCoordinates center() const;

    void setWidth( qreal width );

    qreal width() const;

    void setHeight( qreal height );

    qreal height() const;

    void setIsGeoProjected( bool isGeoProjected );

    bool isGeoProjected() const;

    void setViewport( const ViewportParams *viewport );

    void paint( GeoPainter* painter ) const;

protected:
    GeoDataLatLonAltBox m_latLonAltBox;
    GeoDataCoordinates m_center;
    qreal m_width;
    qreal m_height;
    bool m_isGeoProjected;
};

}

#endif
