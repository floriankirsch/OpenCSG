// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2006-2016, Florian Kirsch
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
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

//
// context.h 
//
// OpenCSG context management
//

#ifndef __OpenCSG__context_h__
#define __OpenCSG__context_h__

#include "opencsgConfig.h"
#include <opencsg.h>
#include <GL/glew.h>

namespace OpenCSG {

    /// redeclared from opencsg.h
    void setContext(int context);
    /// redeclared from opencsg.h
    int getContext();
    /// redeclared from opencsg.h
    void freeResources();

    namespace OpenGL {

        class OffscreenBuffer;

        /// Checks the OpenGL-extensions resp. the current settings
        /// and returns a concrete offscreen buffer, for the currently
        /// active context in OpenCSG.
        OffscreenBuffer* getOffscreenBuffer(OffscreenType type);

        /// Given a constant(!) ARB vertex program string and its length,
        /// returns a ARB vertex program object, for the currently
        /// active context in OpenCSG.
        GLuint getARBVertexProgram(const char* prog, int len);

        /// Given a constant(!) ARB fragment program string and its length,
        /// returns a ARB fragment program object, for the currently
        /// active context in OpenCSG.
        GLuint getARBFragmentProgram(const char* prog, int len);

        /// Frees all resources (offscreen buffers, fragment programs...)
        /// allocated for the currently active context in OpenCSG.
        void freeResources();

    } // namespace OpenGL

} // namespace OpenCSG

#endif // __OpenCSG__context_h__

