// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2010-2011, Florian Kirsch
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
// frameBufferObjectExt.cpp 
//

#include "opencsgConfig.h"
#include "frameBufferObjectExt.h"

namespace OpenCSG {

    namespace OpenGL {

        // ctor / dtor
        FrameBufferObjectExt::FrameBufferObjectExt()
          : width(-1),
            height(-1),
            textureID(0),
            depthID(0),
            framebufferID(0),
            oldFramebufferID(0),
            initialized(false)
        {
        }

        FrameBufferObjectExt::~FrameBufferObjectExt() {
            Reset();
        }

        // Creates frame buffer texture and combined depth/stencil render buffer.
        // shareObjects and copyContext do not make sense here, context remains the same.
        bool FrameBufferObjectExt::Initialize(int width, int height, bool /* shareObjects */, bool /* copyContext */ ) {

            bool haveFBO =    GLEW_EXT_framebuffer_object != 0 
                           && GLEW_EXT_packed_depth_stencil != 0;
            if (!haveFBO)
                return false;

            this->width = width;
            this->height = height;

            glGenFramebuffersEXT(1, &framebufferID);
            glGenRenderbuffersEXT(1, &depthID); 
            glGenTextures(1, &textureID);

            glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &oldFramebufferID);
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebufferID);

            GLenum target = (GLEW_ARB_texture_rectangle || GLEW_EXT_texture_rectangle || GLEW_NV_texture_rectangle)
                ? GL_TEXTURE_RECTANGLE_ARB
                : GL_TEXTURE_2D; // implicitely asks for GL_ARB_texture_non_power_of_two.
                                 // this should have been checked in channelManager.cpp

            glBindTexture(target, textureID);
            glTexImage2D(target, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_INT, 0);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, textureID, 0);

            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthID);
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL_EXT, width, height);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthID);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthID);

            GLenum status;
            status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
            if (status == GL_FRAMEBUFFER_UNSUPPORTED_EXT) {
                Reset();
                return false;
            }

            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, oldFramebufferID);
            glBindTexture(target, 0);

            textureTarget = target;

            initialized = true;

            return true;
        }

        // Releases frame buffer objects
        bool FrameBufferObjectExt::Reset()
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

            this->width = -1;
            this->height = -1;

            initialized = false;

            return true;
        }

        // If new requested size differs, regenerate FBO texture objects
        bool FrameBufferObjectExt::Resize(int width, int height)
        {
            if (   this->width == width
                && this->height == height
            ) {
                return true;
            }

            Reset();
            return Initialize(width, height);
        }

        // Binds the created frame buffer texture such we can render into it.
        bool FrameBufferObjectExt::BeginCapture()
        {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebufferID);
            return true;
        }

        // Unbinds frame buffer texture.
        bool FrameBufferObjectExt::EndCapture()
        {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, oldFramebufferID);
            return true;
        }

        // Sets the frame buffer texture as active texture object.
        void FrameBufferObjectExt::Bind() const
        {
            glBindTexture(textureTarget, textureID);
        }

    } // namespace OpenGL

} // namespace OpenCSG
