// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2006, Florian Kirsch
//
// This library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU General Public License, 
// Version 2, as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

//
// offscreenBuffer.cpp 
//

#include "opencsgConfig.h"
#include "offscreenBuffer.h"
#include "frameBufferObject.h"
#include "pBufferTexture.h"

namespace OpenCSG {

    namespace OpenGL {

        OffscreenBuffer* getOffscreenBuffer(bool fbo) {
            static FrameBufferObject* f = new FrameBufferObject;
            static PBufferTexture* p = new PBufferTexture;
            
            if (fbo)
                return f;
            else 
                return p;
        }

    } // namespace OpenGL

} // namespace OpenCSG
