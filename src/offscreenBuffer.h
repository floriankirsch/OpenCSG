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
// offscreenBuffer.h 
//
// offscreen buffer abstraction
//

#ifndef __OpenCSG__offscreen_buffer_h__
#define __OpenCSG__offscreen_buffer_h__

#include "opencsgConfig.h"

namespace OpenCSG {

    namespace OpenGL {

        class OffscreenBuffer {

        public: // abstract base class

            virtual ~OffscreenBuffer() {};

            /// Call this once before use.  Set bShare to true to share lists, textures, 
            /// and program objects between the render texture context and the 
            /// current active GL context.
            virtual bool Initialize(int width, int height, 
                                    bool shareObjects=true, 
                                    bool copyContext=false) = 0;

            /// Change the render texture format.
            virtual bool Reset() = 0;
            /// Change the size of the render texture. After this, Initialize
            /// must be called again.
            virtual bool Resize(int width, int height) = 0;

            /// Begin drawing to the texture. (i.e. use as "output" texture)
            virtual bool BeginCapture() = 0;
            /// End drawing to the texture.
            virtual bool EndCapture() = 0;

            /// Bind the texture to the active texture unit for use as an "input" texture
            virtual void Bind() const = 0;

            /// Enables the texture target appropriate for this render texture.
            virtual void EnableTextureTarget() const = 0;
            /// Disables the texture target appropriate for this render texture.
            virtual void DisableTextureTarget() const = 0;

            /// Returns the texture target this texture is bound to.
            virtual unsigned int GetTextureTarget() const = 0;
            /// Returns the width of the offscreen buffer.
            virtual int GetWidth() const = 0;
            /// Returns the width of the offscreen buffer.
            virtual int GetHeight() const = 0;

            /// Returns whether the offscreen buffer has a rendering context different from
            /// the rendering context of the caller or not.
            virtual bool haveSeparateContext() const = 0;
        };

        /// checks the OpenGL-extensions resp. the current settings
        /// and returns a concrete offscreen buffer.
        OffscreenBuffer* getOffscreenBuffer(bool fbo);

    } // namespace OpenGL

} // namespace OpenCSG

#endif // __OpenCSG__offscreen_buffer_h__
