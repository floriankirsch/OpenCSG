// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2002-2006
// Hasso-Plattner-Institute at the University of Potsdam, Germany, and Florian Kirsch
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
// frameBufferObject.h 
//
// frame buffer object class implementing the offscreen buffer interface
//

#ifndef __OpenCSG__frame_buffer_object_h__
#define __OpenCSG__frame_buffer_object_h__

#include "opencsgConfig.h"
#include "offscreenBuffer.h"
#include <GL/glew.h>

namespace OpenCSG {

    namespace OpenGL {

        class FrameBufferObject : public OffscreenBuffer {
        public: // interface
            // ctor / dtor
            FrameBufferObject();
            virtual ~FrameBufferObject();
    
            //! Call this once before use.  Set bShare to true to share lists, textures, 
            //! and program objects between the render texture context and the 
            //! current active GL context.
            virtual bool Initialize(int width, int height, 
                            bool shareObjects=true, 
                            bool copyContext=false);

            // !Change the render texture format.
            virtual bool Reset();
            // !Change the size of the render texture.
            virtual bool Resize(int width, int height);
    
            // !Begin drawing to the texture. (i.e. use as "output" texture)
            virtual bool BeginCapture();
            // !End drawing to the texture.
            virtual bool EndCapture();
    
            // !Bind the texture to the active texture unit for use as an "input" texture
            virtual void Bind() const;

            //! Enables the texture target appropriate for this render texture.
            virtual void EnableTextureTarget() const 
            { if (initialized) glEnable(textureTarget); }
            //! Disables the texture target appropriate for this render texture.
            virtual void DisableTextureTarget() const 
            { if (initialized) glDisable(textureTarget); }

            //! Returns the texture target this texture is bound to.
            virtual unsigned int GetTextureTarget() const { return textureTarget; }
            //! Returns the width of the offscreen buffer.
            virtual int GetWidth() const  { return width;  } 
            //! Returns the width of the offscreen buffer.
            virtual int GetHeight() const { return height; }

            virtual bool haveSeparateContext() const {
                return false;
            }

        protected: // data
            int          width;     // width of the pbuffer
            int          height;    // height of the pbuffer
    
            // Texture stuff
            GLenum       textureTarget;
            unsigned int textureID;
            unsigned int depthID;

            unsigned int framebufferID;

            bool initialized;
        };

    } // namespace OpenGL

} // namespace OpenCSG

#endif // __OpenCSG__frame_buffer_object_h__
