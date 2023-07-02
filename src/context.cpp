// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2006-2022, Florian Kirsch
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110 - 1301 USA.

//
// context.cpp
//

#include "opencsgConfig.h"
#include "context.h"
#include "offscreenBuffer.h"
#include "frameBufferObject.h"
#include "frameBufferObjectExt.h"
#include "openglHelper.h"
#include <map>

namespace OpenCSG {

    static int gContext = 0;

    /// redeclared from opencsg.h
    void setContext(int context) {
        gContext = context;
    }

    /// redeclared from opencsg.h
    int getContext() {
        return gContext;
    }

    /// redeclared from opencsg.h
    void freeResources() {
        OpenGL::freeResources();
    }

    namespace OpenGL {

        struct ContextData {
            ContextData() : fARB(0), fEXT(0)
          {}
            FrameBufferObject* fARB;
            FrameBufferObjectExt* fEXT;
            std::map<const char*, GLuint> idFP;
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
            return 0;
        }

        GLuint getARBProgram(GLenum target, const char* prog, int len)
        {
            int context = getContext();
            ContextData& contextData = gContextDataMap[context];

            std::map<const char*, GLuint>::iterator it = contextData.idFP.find(prog);
            if (it == contextData.idFP.end())
            {
                GLuint id;
                glGenProgramsARB(1, &id);
                glBindProgramARB(target, id);
                glProgramStringARB(target, GL_PROGRAM_FORMAT_ASCII_ARB, len, prog);

                // GLint errorPos;
                // glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errorPos);
                // const char * error = (const char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB);
                // printf("%s", error);

                it = contextData.idFP.insert(std::pair<const char*, GLuint>(prog, id)).first;
            }

            return it->second;
        }

        GLuint getARBVertexProgram(const char* prog, int len)
        {
            return getARBProgram(GL_VERTEX_PROGRAM_ARB, prog, len);
        }

        GLuint getARBFragmentProgram(const char* prog, int len)
        {
            return getARBProgram(GL_FRAGMENT_PROGRAM_ARB, prog, len);
        }

        void freeResources()
        {
            int context = getContext();
            std::map<int, ContextData>::iterator itr = gContextDataMap.find(context);
            if (itr != gContextDataMap.end())
            {
                delete itr->second.fARB;
                delete itr->second.fEXT;
                std::map<const char*, GLuint> & idFP = itr->second.idFP;
                for (std::map<const char*, GLuint>::iterator it = idFP.begin(); it != idFP.end(); ++it)
                {
                    glDeleteProgramsARB(1, &(it->second));
                }
                gContextDataMap.erase(itr);
            }
        }

    } // namespace OpenGL

} // namespace OpenCSG
