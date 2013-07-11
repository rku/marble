//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#ifndef APRSOBJECT_H
#define APRSOBJECT_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include "GeoAprsCoordinates.h"
#include "GeoDataStyle.h"

namespace Marble
{

    class GeoDataLineString;

    class AprsObject
    {

      public:
        AprsObject();
        AprsObject( const GeoAprsCoordinates &at, QString &name );
        AprsObject( const qreal &lon, const qreal &lat, const QString &name,
                    int where = GeoAprsCoordinates::FromNowhere );
        ~AprsObject();

        const QString name() const;

        const GeoDataLineString* trackLine() const;

        void setLocation( GeoAprsCoordinates location );
        void setLocation( qreal lon, qreal lat, int from );
        GeoAprsCoordinates location();

        void setPixmapId( QString &pixmap );
        QString pixmapId() const;
        QPixmap* pixmap() const;

        void setSeenFrom( int where );
        int seenFrom() const;

        void update( int fadeTime = 10*60, int hideTime = 30*60 );

      private:
        QColor calculatePaintColor( int from, const QTime &time,
                                    int fadetime = 10*60*1000 ) const;

        QString                       m_name;
        int                           m_seenFrom;
        bool                          m_havePixmap;
        QString                       m_pixmapFilename;
        QPixmap                      *m_pixmap;
        GeoDataLineString            *m_trackLine;
        QList<GeoAprsCoordinates>     m_history;
    };

}

#endif /* APRSOBJECT_H */
