// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2010-2022, Florian Kirsch
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
// frameBufferObjectExt.h 
//
// frame buffer object class implementing the offscreen buffer interface
//

#ifndef __OpenCSG__frame_buffer_object_ext_h__
#define __OpenCSG__frame_buffer_object_ext_h__

#include "opencsgConfig.h"
#include "offscreenBuffer.h"
#include <GL/glew.h>

namespace OpenCSG {

    namespace OpenGL {

        class FrameBufferObjectExt : public OffscreenBuffer {
        public:
            /// ctor / dtor
            FrameBufferObjectExt();
            virtual ~FrameBufferObjectExt();

            /// Reads the currently bound FBO.
            virtual bool ReadCurrent();

            /// Call this once before use. shareObjects and copyContext are
            /// ignored, since there are no separate contexts for frame
            /// buffer objects anyway.
            virtual bool Initialize(int width, int height, bool shareObjects=true, bool copyContext=false);

            /// checks whether Initialize has been called before or not
            virtual bool IsInitialized() const { return initialized; }

            /// Removes the frame buffer object OpenGL resources.
            virtual bool Reset();
            /// Change the size of the frame buffer object.
            virtual bool Resize(int width, int height);

            /// Begin drawing to the frame buffer object. (i.e. use as "output" texture)
            virtual bool BeginCapture();
            /// End drawing to the frame buffer object.
            virtual bool EndCapture();

            /// Bind the frame buffer object to the active texture unit for use as an "input" texture
            virtual void Bind() const;

            /// Enables the texture target appropriate for this frame buffer object.
            virtual void EnableTextureTarget() const { if (initialized) glEnable(GL_TEXTURE_2D); }
            /// Disables the texture target appropriate for this frame buffer object.
            virtual void DisableTextureTarget() const { if (initialized) glDisable(GL_TEXTURE_2D); }

            /// Returns the width of the frame buffer object.
            virtual int GetWidth() const  { return width;  } 
            /// Returns the width of the frame buffer object.
            virtual int GetHeight() const { return height; }

            /// Frame buffer objects do not change the OpenGL context.
            virtual bool haveSeparateContext() const { return false; }

        protected:
            int          width;     // width of the frame buffer object
            int          height;    // height of the frame buffer object

            /// Texture stuff
            unsigned int textureID;
            unsigned int depthID;

            unsigned int framebufferID;
            int          oldFramebufferID;

            bool         initialized;

        private:
            FrameBufferObjectExt(const FrameBufferObjectExt&);
            FrameBufferObjectExt& operator=(const FrameBufferObjectExt&);

        };

    } // namespace OpenGL

} // namespace OpenCSG

#endif // __OpenCSG__frame_buffer_object_ext_h__
