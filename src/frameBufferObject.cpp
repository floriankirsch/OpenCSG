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
// frameBufferObject.cpp 
//

#include "opencsgConfig.h"
#include "frameBufferObject.h"
#include <cassert>
#include <iostream>

namespace OpenCSG {

    namespace OpenGL {

        // ctor / dtor
        FrameBufferObject::FrameBufferObject()
          : width(-1),
            height(-1),
            textureID(0),
            depthID(0),
            framebufferID(0),
            initialized(false)
        {
        }

        FrameBufferObject::~FrameBufferObject() {
            Reset();
        }

        // Creates frame buffer texture and combined depth/stencil render buffer.
        // shareObjects and copyContext do not make sense here, context remains the same.
        bool FrameBufferObject::Initialize(int width, int height, bool /* shareObjects */, bool /* copyContext */ ) {

            bool haveFBO = GLEW_ARB_framebuffer_object != 0;
            if (!haveFBO)
                return false;

            this->width = width;
            this->height = height;

            glGenFramebuffers(1, &framebufferID);
            glGenRenderbuffers(1, &depthID); 
            glGenTextures(1, &textureID);

            glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_INT, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

            glBindRenderbuffer(GL_RENDERBUFFER, depthID);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthID);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthID);

            GLenum status;
            status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status == GL_FRAMEBUFFER_UNSUPPORTED) {
                Reset();
                return false;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);

            textureTarget = GL_TEXTURE_2D;

            initialized = true;

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
                glDeleteRenderbuffers(1, &depthID); 
                depthID = 0;
            }
            if (framebufferID) {
                glDeleteFramebuffers(1, &framebufferID);
                framebufferID = 0;
            }

            this->width = -1;
            this->height = -1;

            initialized = false;

            return true;
        }

        // If new requested size differs, regenerate FBO texture objects
        bool FrameBufferObject::Resize(int width, int height)
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
        bool FrameBufferObject::BeginCapture()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
            return true;
        }

        // Unbinds frame buffer texture.
        bool FrameBufferObject::EndCapture()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return true;
        }

        // Sets the frame buffer texture as active texture object.
        void FrameBufferObject::Bind() const
        {
            glBindTexture(GL_TEXTURE_2D, textureID);
        }

    } // namespace OpenGL

} // namespace OpenCSG
