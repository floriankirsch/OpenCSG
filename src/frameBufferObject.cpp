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
// frameBufferObject.cpp 
//

#include "opencsgConfig.h"
#include "frameBufferObject.h"
#include <cassert>
#include <iostream>

#define CHECK_FRAMEBUFFER_STATUS()                            \
{                                                             \
    GLenum status;                                            \
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); \
    switch(status) {                                          \
      case GL_FRAMEBUFFER_COMPLETE_EXT:                       \
        /*std::cout << "framebuffer complete" << std::endl;*/ \
        break;                                                \
      case GL_FRAMEBUFFER_UNSUPPORTED_EXT:                    \
        /*std::cout << "framebuffer unsupported" << std::endl;*/\
        /* choose different formats */                        \
        break;                                                \
      default:                                                \
        /* programming error; will fail on all hardware */    \
        assert(0);                                            \
    }                                                         \
}

namespace OpenCSG {

    namespace OpenGL {

        // ctor / dtor
        FrameBufferObject::FrameBufferObject()
          : width(-1),
            height(-1),
            textureID(0),
            depthID(0),
            framebufferID(0)
        {
        }

        FrameBufferObject::~FrameBufferObject() {
            Reset();
        }

        // Creates frame buffer texture and combined depth/stencil render buffer.
        // shareObjects and copyContext do not make sense here, context remains the same.
        bool FrameBufferObject::Initialize(int width, int height, bool /* shareObjects */, bool /* copyContext */ ) {

            bool haveFBO =    GLEW_EXT_framebuffer_object != 0 
                           && GLEW_EXT_packed_depth_stencil != 0;

            assert(haveFBO);

            this->width = width;
            this->height = height;

            glGenFramebuffersEXT(1, &framebufferID);
            glGenRenderbuffersEXT(1, &depthID); 
            glGenTextures(1, &textureID);

            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebufferID);	
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_INT, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, textureID, 0);

            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthID);
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL_NV, width, height);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthID);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthID);
	        
            CHECK_FRAMEBUFFER_STATUS();
    
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);	
            glBindTexture(GL_TEXTURE_2D, 0);

            textureTarget = GL_TEXTURE_2D;

            return true;
        }

        // Releases frame buffer objects
        bool FrameBufferObject::Reset()
        {
            if (textureID) {
                glDeleteTextures(1, &textureID);
                textureID = 0;
            }
            if (depthID) {
                glDeleteRenderbuffersEXT(1, &depthID); 
                depthID = 0;
            }
            if (framebufferID) {
                glDeleteFramebuffersEXT(1, &framebufferID);
                framebufferID = 0;
            }
            return true;
        }

        // ?
        bool FrameBufferObject::Resize(int /* width */, int /* height */ )
        {
            Reset();
            return true;
        }

        // Binds the created frame buffer texture such we can render into it.
        bool FrameBufferObject::BeginCapture()
        {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebufferID);
            return true;
        }

        // Unbinds frame buffer texture.
        bool FrameBufferObject::EndCapture()
        {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
            return true;
        }

        // Sets the frame buffer texture as active texture object.
        void FrameBufferObject::Bind() const
        {
            glBindTexture(GL_TEXTURE_2D, textureID);
        }

    } // namespace OpenGL

} // namespace OpenCSG
