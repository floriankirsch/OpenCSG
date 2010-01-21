// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2006-2010, Florian Kirsch
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
#ifdef _WIN32
#include <GL/wglew.h>
#endif

namespace OpenCSG {

    namespace OpenGL {

        PBufferTexture::PBufferTexture() {
            if (GLEW_NV_texture_rectangle) {
#ifdef _WIN32
                if (WGLEW_ARB_render_texture) {
                    s = "rgba texRECT depth=24 stencil=8 single rtt";
                } else
#endif
                { 
                    s = "rgba texRECT depth=24 stencil=8 single ctt"; 
                }
            } else {
#ifdef _WIN32
                if (WGLEW_ARB_render_texture) {
                    s = "rgba tex2D depth=24 stencil=8 single rtt";
                } else 
#endif
                {
                    s = "rgba tex2D depth=24 stencil=8 single ctt";
                }
            }

            r = new RenderTexture(s);
        }

        PBufferTexture::~PBufferTexture() {
            delete r;
        }

        bool PBufferTexture::Initialize(int width, int height, bool shareObjects, bool copyContext) {
            return r->Initialize(width, height, shareObjects, copyContext);
        }

        bool PBufferTexture::IsInitialized() const {
            return r->IsInitialized();
        }

        bool PBufferTexture::Reset() {
            return r->Reset(s);
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

