// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2006-2010, Florian Kirsch
//
// This library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU General Public License, 
// Version 2, as published by the Free Software Foundation.
// As a special exception, you have permission to link this library
// with the CGAL library and distribute executables.
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
#include "context.h"
#include "offscreenBuffer.h"
#include "frameBufferObject.h"
#include "frameBufferObjectExt.h"
#include "pBufferTexture.h"
#include <GL/glew.h>
#include <map>

namespace OpenCSG {

    namespace OpenGL {

        struct ContextData {
            ContextData() : fARB(0), fEXT(0), pBuf(0) {}
            FrameBufferObject* fARB;
            FrameBufferObjectExt* fEXT;
            PBufferTexture* pBuf;
        };

        static std::map<int, ContextData> gContextDataMap;

        OffscreenBuffer* getOffscreenBuffer(OffscreenType type) {
            int context = getContext();
            ContextData& contextData = gContextDataMap[context];

            if (type == OpenCSG::FrameBufferObjectARB) {
                if (!contextData.fARB)
                    contextData.fARB = new FrameBufferObject;
                return contextData.fARB;
            }
            else if (type == OpenCSG::FrameBufferObjectEXT) {
                if (!contextData.fEXT)
                    contextData.fEXT = new FrameBufferObjectExt;
                return contextData.fEXT;
            }
            else if (type == OpenCSG::PBuffer) {
                if (!contextData.pBuf)
                    contextData.pBuf = new PBufferTexture;
                return contextData.pBuf;
            }

            return 0;
        }

        void freeResources() {
            int context = getContext();
            std::map<int, ContextData>::iterator itr = gContextDataMap.find(context);
            if (itr != gContextDataMap.end()) {
                delete itr->second.fARB;
                delete itr->second.fEXT;
                delete itr->second.pBuf;
                gContextDataMap.erase(itr);
            }
        }

    } // namespace OpenGL

} // namespace OpenCSG
