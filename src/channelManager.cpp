// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2002-2004
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
// channelManager.cpp
//

#include <opencsgConfig.h>
#include <GL/glew.h>

#ifdef _WIN32
#include <GL/wglew.h>
#else // _WIN32 not defined, assume glX
#include <GL/glxew.h>
#endif // _WIN32

#include "channelManager.h"
#include "openglHelper.h"
#include "RenderTexture/RenderTexture.h"
#include <cassert>

namespace OpenCSG {
    
    RenderTexture* ChannelManager::pbuffer_ = 0;
    bool ChannelManager::inUse_ = false;
    
    namespace {

        GLint FaceOrientation = GL_CCW;

        int nextPow2(int value) {
            if(value <= 0) { return 0; }
            int result = 1;
            while(result < value) {
                result <<= 1;
            }
            return result;
        }

        void defaults() {
            glViewport(OpenGL::canvasPos[0], OpenGL::canvasPos[1], OpenGL::canvasPos[2], OpenGL::canvasPos[3]);
            glClearColor(0.0, 0.0, 0.0, 0.0);
            glClearDepth(1.0);
            glClearStencil(0);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glDepthMask(GL_TRUE);
            glStencilMask(0xffffffff);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
        }

    } // unnamed namespace

    ChannelManager::ChannelManager() {

        assert(!inUse_);      
        inUse_ = true;

        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDisable(GL_LIGHTING);
        glShadeModel(GL_FLAT);

        glGetIntegerv(GL_FRONT_FACE, &FaceOrientation);

        glGetFloatv(GL_MODELVIEW_MATRIX, OpenGL::modelview);
        glGetFloatv(GL_PROJECTION_MATRIX, OpenGL::projection);
        glGetIntegerv(GL_VIEWPORT, OpenGL::canvasPos);

        if (glIsEnabled(GL_SCISSOR_TEST)) {
            glGetIntegerv(GL_SCISSOR_BOX, OpenGL::scissorPos);
        } else {
            OpenGL::scissorPos[0] = OpenGL::canvasPos[0];
            OpenGL::scissorPos[1] = OpenGL::canvasPos[1];
            OpenGL::scissorPos[2] = OpenGL::canvasPos[2];
            OpenGL::scissorPos[3] = OpenGL::canvasPos[3];
        }

        const int dx = OpenGL::canvasPos[2] - OpenGL::canvasPos[0];
        const int dy = OpenGL::canvasPos[3] - OpenGL::canvasPos[1];

        int tx = dx;
        int ty = dy;
        if (!GLEW_NV_texture_rectangle) {
            // blow up the texture to legal power-of-two size :-(
            tx = nextPow2(dx);
            ty = nextPow2(dy);
        }

        // these variables are for a heuristic that makes the pbuffer smaller
        // if the size of the pbuffer has been bigger than necessary in 
        // x- or y- direction for resizePBufferLimit frames. 
        //
        // this permits to use OpenCSG for CSG rendering in different
        // canvases with different sizes without constant expensive
        // resizing of the pbuffer for every frame.
        //
        // possible improvements: 
        //   - the algorithm is not beautifully implemented. maybe encapsulate it?
        //   - allow the user to define the resizePBufferLimit?
        static const unsigned int resizePBufferLimit = 100;
        static unsigned int resizePBufferCounterX = 0;
        static unsigned int resizePBufferCounterY = 0;
        static int maxPBufferSizeX = 1;
        static int maxPBufferSizeY = 1;

        bool rebuild = false;
        if (pbuffer_ == 0) {
        
            if (GLEW_NV_texture_rectangle) {
                pbuffer_ = new RenderTexture("rgba texRECT depth=24 stencil=8 single");
            } else {
                pbuffer_ = new RenderTexture("rgba tex2D depth=24 stencil=8 single");
            }
            rebuild = true;
        // tx == ty == 0 happens if the window is minimized, in this case don't touch a thing
        } else if (tx != 0 && ty != 0) {
            
            // check whether the pbuffer is too small, in case resize immediately
            if ((tx > pbuffer_->GetWidth()) || (ty > pbuffer_->GetHeight())) {
                pbuffer_->Resize(std::max(maxPBufferSizeX, tx), std::max(maxPBufferSizeY, ty));
                rebuild = true;
            }           

            // if x-size matches exactly, remember to not resize in the next resizePBufferLimit frames
            if (tx == pbuffer_->GetWidth()) {
                resizePBufferCounterX = 0;
                maxPBufferSizeX = tx;
            }
            // else remember the biggest X in the last resizePBufferCounterX frames
              else if (tx < pbuffer_->GetWidth()) {
                ++resizePBufferCounterX;
                maxPBufferSizeX = std::max(maxPBufferSizeX, tx);
            }

            // if y-size matches exactly, remember to not resize in the next resizePBufferLimit frames
            if (ty == pbuffer_->GetHeight()) {
                resizePBufferCounterY = 0;
                maxPBufferSizeY = ty;
            } 
            // else remember the biggest Y in the last resizePBufferCounterY frames
            else if (ty < pbuffer_->GetHeight()) {
                ++resizePBufferCounterY;
                maxPBufferSizeY = std::max(maxPBufferSizeY, ty);
            }

            // X or Y have been smaller than the PBuffer-Size in the last resizePBufferLimit frames
            // -> resize!
            if (resizePBufferCounterX >= resizePBufferLimit || resizePBufferCounterY >= resizePBufferLimit) {
                if (maxPBufferSizeX != pbuffer_->GetWidth() || maxPBufferSizeY != pbuffer_->GetHeight()) {
                    // a beautiful algorithm would ensure that this inner if-condition 
                    // would always be fulfilled.  
                    pbuffer_->Resize(maxPBufferSizeX, maxPBufferSizeY);
                    rebuild = true;
                }
                resizePBufferCounterX = 0;
                resizePBufferCounterY = 0;
                maxPBufferSizeX = tx;
                maxPBufferSizeY = ty;
            }

        }

        if (rebuild) {
            // should not be required after Resize() - but in practice I get errors (20.07.2004, Quadro FX500, 61.40)
            if (!pbuffer_->Initialize(tx, ty, true, false)) {
                assert(0);
            }

            resizePBufferCounterX = 0;
            resizePBufferCounterY = 0;
            maxPBufferSizeX = tx;
            maxPBufferSizeY = ty;

            assert(pbuffer_->HasStencil());

            pbuffer_->BeginCapture();
            defaults();
            pbuffer_->EndCapture();
            pbuffer_->Bind();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        inPBuf_ = false;
        currentChannel_ = NoChannel;
        occupiedChannels_ = NoChannel;
    }

    ChannelManager::~ChannelManager() {
        glPopAttrib();
        assert(inUse_);
        inUse_ = false;
    }

    Channel ChannelManager::find() const {

        Channel channel_ = NoChannel;

        // find free channel
        if ((occupiedChannels_ & Alpha) == 0) {
            channel_ = Alpha;
        }  else if (GLEW_ARB_texture_env_dot3) {
            if ((occupiedChannels_ & Red) == 0)   {
                channel_ = Red;
            } else if ((occupiedChannels_ & Green) == 0) {
                channel_ = Green;
            } else if ((occupiedChannels_ & Blue) == 0)  {
                channel_ = Blue;
            }
        }

        return channel_;
    }

    Channel ChannelManager::request() {
        if (!inPBuf_) {
            pbuffer_->BeginCapture();
            inPBuf_ = true;

            glGetIntegerv(GL_STENCIL_BITS, &OpenGL::stencilBits);
            OpenGL::stencilMax = 1 << OpenGL::stencilBits;
            OpenGL::stencilMask = OpenGL::stencilMax - 1;

            glFrontFace(FaceOrientation);

            currentChannel_ = NoChannel;
            occupiedChannels_ = NoChannel;
        }

        glViewport(OpenGL::canvasPos[0], OpenGL::canvasPos[1], OpenGL::canvasPos[2], OpenGL::canvasPos[3]);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(OpenGL::projection);
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(OpenGL::modelview);

        currentChannel_ = find();
        occupiedChannels_ |= currentChannel_;
        return currentChannel_;
    }

    Channel ChannelManager::current() const {
        return currentChannel_;
    }

    std::vector<Channel> ChannelManager::occupied() const {

        std::vector<Channel> result;

        if ((occupiedChannels_ & Alpha) != 0) {
            result.push_back(Alpha);
        }        
        if ((occupiedChannels_ & Red) != 0) {
            result.push_back(Red);
        }        
        if ((occupiedChannels_ & Green) != 0) {
            result.push_back(Green);
        }        
        if ((occupiedChannels_ & Blue) != 0) {
            result.push_back(Blue);
        }
     
        return result;
    }

    void ChannelManager::free() {
        if (inPBuf_) {
            pbuffer_->EndCapture();
            inPBuf_ = false;
        }

        merge();
    }

    void ChannelManager::renderToChannel(bool on) {

        if (on) {
            switch (currentChannel_) {
                case NoChannel:
                    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                    break;
                case Alpha:
                    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
                    break;
                case Blue:
                    glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE);
                    break;        
                case Green:
                    glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_FALSE);
                    break;         
                case Red:
                    glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE);
                    break;
                }
            }
        else {
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        }
    }


    void ChannelManager::setupProjectiveTexture() {
        static float splane[4] = { 1.0, 0.0, 0.0, 0.0 };
        static float tplane[4] = { 0.0, 1.0, 0.0, 0.0 };
        static float rplane[4] = { 0.0, 0.0, 1.0, 0.0 };
        static float qplane[4] = { 0.0, 0.0, 0.0, 1.0 };

        pbuffer_->Bind();
        pbuffer_->EnableTextureTarget();

        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGenfv(GL_S, GL_EYE_PLANE, splane);
        glTexGenfv(GL_T, GL_EYE_PLANE, tplane);
        glTexGenfv(GL_R, GL_EYE_PLANE, rplane);
        glTexGenfv(GL_Q, GL_EYE_PLANE, qplane);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_GEN_R);
        glEnable(GL_TEXTURE_GEN_Q);

        glMatrixMode(GL_TEXTURE);

        const int dx = OpenGL::canvasPos[2] - OpenGL::canvasPos[0];
        const int dy = OpenGL::canvasPos[3] - OpenGL::canvasPos[1];

        // with NV_texture_rectangle texture coordinates range between
        // 0 and dx resp. dy
        float factorX = static_cast<float>(dx);
        float factorY = static_cast<float>(dy);

        // RenderTexture has a slight flaw: if you request a power-of-two texture
        // by coincidence even though you expected a NPOT texture, you will not
        // automatically notice that. That's for a bug that only happened at
        // 512x512 canvas resolution. 
        // Therefore we do not check for the extension, but simply for the texture format
        // Update (08.04.2004): Fixed the flaw, but kept checking the texture format.
        // Actually that seems safer, since it should work always
        if (pbuffer_->GetTextureTarget() == GL_TEXTURE_2D) {
            // with ordinary pow-of-two texture coordinates are between 0 and 1
            // but we must assure only the used part of the texture is taken.
            factorX /= static_cast<float>(pbuffer_->GetWidth());
            factorY /= static_cast<float>(pbuffer_->GetHeight());
        }

        float   texCorrect[16] = { factorX, 0.0, 0.0, 0.0, 
                                   0.0, factorY, 0.0, 0.0, 
                                   0.0,     0.0, 1.0, 0.0,
                                   0.0,     0.0, 0.0, 1.0 };
        
        static float p2ndc[16] = { 0.5, 0.0, 0.0, 0.0, 
                                   0.0, 0.5, 0.0, 0.0, 
                                   0.0, 0.0, 0.5, 0.0, 
                                   0.5, 0.5, 0.5, 1.0 };
        glPushMatrix();
        glLoadMatrixf(texCorrect);
        glMultMatrixf(p2ndc);
        glMultMatrixf(OpenGL::projection);
        glMultMatrixf(OpenGL::modelview);
        glMatrixMode(GL_MODELVIEW);
    }

    void ChannelManager::resetProjectiveTexture() {
        glMatrixMode(GL_TEXTURE);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        pbuffer_->DisableTextureTarget();
    }

    void ChannelManager::setupTexEnv(Channel channel) {

        if (channel == Alpha) {
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        } else {
            // replicate color into alpha
            if (GLEW_ARB_texture_env_dot3) {
                switch (channel) {
                case Red: 
                    glColor3f(1.0, 0.5, 0.5); 
                    break;                
                case Green: 
                    glColor3f(0.5, 1.0, 0.5); 
                    break;
                case Blue: 
                    glColor3f(0.5, 0.5, 1.0); 
                    break;
                default:
                    // should not happen!
                    assert(0);
                }
            } else {
                // should not happen!
                assert(0);
            }

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_DOT3_RGBA_ARB);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PRIMARY_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
        }
    }



    
    ChannelManagerForBatches::ChannelManagerForBatches() : 
        ChannelManager(), 
        primitives_(std::vector<std::pair<std::vector<Primitive*>, int> >(Blue + 1)) {
    }

    void ChannelManagerForBatches::store(Channel channel, const std::vector<Primitive*>& primitives, int layer) {
        primitives_[channel] = std::make_pair(primitives, layer);
    }

    const std::vector<Primitive*> ChannelManagerForBatches::getPrimitives(Channel channel) const {
        return primitives_[channel].first;
    }    
    
    int ChannelManagerForBatches::getLayer(Channel channel) const {
        return primitives_[channel].second;
    }

    void ChannelManagerForBatches::clear() {
        primitives_ = std::vector<std::pair<std::vector<Primitive*>, int> >(Blue + 1);
    }

} // namespace OpenCSG

