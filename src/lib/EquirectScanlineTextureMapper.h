//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
// Copyright 2008      Inge Wallin      <inge@lysator.liu.se>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_EQUIRECTSCANLINETEXTUREMAPPER_H
#define MARBLE_EQUIRECTSCANLINETEXTUREMAPPER_H


#include "TextureMapperInterface.h"

#include "MarbleGlobal.h"

#include <QtCore/QThreadPool>
#include <QtGui/QImage>


namespace Marble
{

class EquirectScanlineTextureMapper : public TextureMapperInterface
{
 public:
    QRect rect( const ViewportParams *viewport ) const;

    void mapTexture( QImage *canvasImage, StackedTileLoader *tileLoader, const ViewportParams *viewport, int tileZoomLevel, MapQuality mapQuality );

 private:
    class RenderJob;

    QThreadPool m_threadPool;
};

}

#endif
