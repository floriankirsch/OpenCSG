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
// pBufferTexture.cpp 
//

#include "opencsgConfig.h"
#include "pBufferTexture.h"
#include "RenderTexture/RenderTexture.h"
#include <GL/glew.h>

namespace OpenCSG {

    namespace OpenGL {

        PBufferTexture::PBufferTexture() {
            if (GLEW_NV_texture_rectangle) {
                r = new RenderTexture("rgba texRECT depth=24 stencil=8 single");
            } else {
                r = new RenderTexture("rgba tex2D depth=24 stencil=8 single");
            }
        }

        PBufferTexture::~PBufferTexture() {
            delete r;
        }

        bool PBufferTexture::Initialize(int width, int height, bool shareObjects, bool copyContext) {
            return r->Initialize(width, height, shareObjects, copyContext);
        }

        bool PBufferTexture::Reset() {
            if (GLEW_NV_texture_rectangle) {
                return r->Reset("rgba texRECT depth=24 stencil=8 single");
            } else {
                return r->Reset("rgba tex2D depth=24 stencil=8 single");
            }
        }

        bool PBufferTexture::Resize(int width, int height) {
            return r->Resize(width, height);
        }

        bool PBufferTexture::BeginCapture() {
            return r->BeginCapture();
        }

        bool PBufferTexture::EndCapture() {
            return r->EndCapture();
        }

        void PBufferTexture::Bind() const {
            r->Bind();
        }

        void PBufferTexture::EnableTextureTarget() const { 
            r->EnableTextureTarget(); 
        }

        void PBufferTexture::DisableTextureTarget() const { 
            r->DisableTextureTarget();
        }

        unsigned int PBufferTexture::GetTextureTarget() const {
            return r->GetTextureTarget();
        }

        int PBufferTexture::GetWidth() const {
            return r->GetWidth();
        }

        int PBufferTexture::GetHeight() const {
            return r->GetHeight();
        }

    } // namespace OpenGL

} // namespace OpenCSG

