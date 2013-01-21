//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "GeoDataTourControl.h"

namespace Marble
{

GeoDataTourControl::GeoDataTourControl() :
    m_id(""),
    m_playMode(Play)
{
}

GeoDataTourControl::~GeoDataTourControl()
{
}

QString GeoDataTourControl::id() const
{
    return m_id;
}

void GeoDataTourControl::setId(const QString &id)
{
    m_id = id;
}

GeoDataTourControl::PlayMode GeoDataTourControl::playMode() const
{
    return m_playMode;
}

void GeoDataTourControl::setPlayMode(const PlayMode &mode)
{
    m_playMode = mode;
}

} // namespace Marble
