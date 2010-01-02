// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2002-2010, Florian Kirsch,
// Hasso-Plattner-Institute at the University of Potsdam, Germany
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

#include "opencsgConfig.h"
#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif

#include "channelManager.h"
#include "offscreenBuffer.h"
#include "openglHelper.h"
#include "settings.h"
#include <cassert>

namespace OpenCSG {

    OpenCSG::OpenGL::OffscreenBuffer* ChannelManager::gOffscreenBuffer = 0;
                                  int ChannelManager::gOffscreenType = OpenCSG::AutomaticOffscreenType;
                                 bool ChannelManager::gInUse = false;
    
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

        template<int FRAMES>
        class MaximumMemorizer {
            int mMax;
            int mSecondMax;
            int mCounter;
        public:
            MaximumMemorizer() : mMax(0), mSecondMax(-1) { }
            void newValue(int v) {
                if (v>=mMax) {
                    mMax = v;
                    mSecondMax = -1;
                    mCounter = 0;
                } else {
                    if (v>mSecondMax) {
                        mSecondMax = v;
                    }
                    if (++mCounter >= FRAMES) {
                        mMax = mSecondMax;
                        mSecondMax = -1;
                        mCounter = 0;
                    }
                }
            }
            int getMax() const {
                return mMax;
            }
        };

    } // unnamed namespace

    ChannelManager::ChannelManager() {

        assert(!gInUse);      
        gInUse = true;

        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_1D);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);

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

        // The following implements a heuristic that makes the offscreen buffer
        // smaller if the size of the buffer has been bigger than necessary
        // in x- or y- direction for resizeOffscreenBufferLimit frames. 
        //
        // this permits to use OpenCSG for CSG rendering in different
        // canvases with different sizes without permanent expensive
        // resizing of the offscreen buffer for every frame.
        //
        // possible improvements: 
        //   - allow the user to define the resizeOffscreenBufferLimit?
        static const unsigned int resizeOffscreenBufferLimit = 64;

        static MaximumMemorizer<resizeOffscreenBufferLimit> sizeX;
        static MaximumMemorizer<resizeOffscreenBufferLimit> sizeY;
        // tx == ty == 0 happens if the window is minimized, in this case don't touch a thing
        if (tx != 0 && ty != 0) {
            sizeX.newValue(tx);
            sizeY.newValue(ty);
        }

        bool rebuild = false;
        int newOffscreenType = getOption(OffscreenSetting);
        if (!gOffscreenBuffer || (gOffscreenType != newOffscreenType)) {
            gOffscreenType = newOffscreenType;
            if (newOffscreenType == OpenCSG::AutomaticOffscreenType) {
                if (GLEW_ARB_framebuffer_object) {
                    newOffscreenType = OpenCSG::FrameBufferObject;
                }
                else 
                if (   GLEW_EXT_framebuffer_object
                    && GLEW_EXT_packed_depth_stencil
                ) {
                    newOffscreenType = OpenCSG::FrameBufferObject;
                }
                else
#ifdef WIN32
                if (   WGLEW_ARB_pbuffer
                    && WGLEW_ARB_pixel_format
#else
                if (   GLXEW_SGIX_pbuffer
                    && GLXEW_SGIX_fbconfig
#endif
                ) {
                    newOffscreenType = OpenCSG::PBuffer;
                }
                else {
                    // This should gracefully exit without doing anything
                    newOffscreenType = OpenCSG::FrameBufferObject;
                }
            }
            if (newOffscreenType == FrameBufferObject) {
                gOffscreenBuffer = OpenGL::getOffscreenBuffer(true);
            } else {
                gOffscreenBuffer = OpenGL::getOffscreenBuffer(false);
            }
            if (   gOffscreenBuffer->GetWidth() != sizeX.getMax()
                || gOffscreenBuffer->GetHeight() != sizeY.getMax()
            ) {
                // in particular, this detects newly created offscreen buffers,
                // of which the width / height is -1
                rebuild = true;
            }
        // tx == ty == 0 happens if the window is minimized, in this case don't touch a thing
        } else if (tx != 0 && ty != 0) {
            if (   gOffscreenBuffer->GetWidth() != sizeX.getMax()
                || gOffscreenBuffer->GetHeight() != sizeY.getMax()
            ) {
                gOffscreenBuffer->Resize(sizeX.getMax(), sizeY.getMax());
                rebuild = true;
            }
        }

        if (rebuild) {
            if (!gOffscreenBuffer->Initialize(sizeX.getMax(), sizeY.getMax(), true, false)) {
                assert(0);
            }

            // assert(pbuffer_->HasStencil());

            gOffscreenBuffer->BeginCapture();
            defaults();
            glGetIntegerv(GL_STENCIL_BITS, &OpenGL::stencilBits);
            OpenGL::stencilMax = 1 << OpenGL::stencilBits;
            OpenGL::stencilMask = OpenGL::stencilMax - 1;
            gOffscreenBuffer->EndCapture();
            gOffscreenBuffer->Bind();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        mInOffscreenBuffer = false;
        mCurrentChannel = NoChannel;
        mOccupiedChannels = NoChannel;
    }

    ChannelManager::~ChannelManager() {
        glPopAttrib();
        assert(gInUse);
        gInUse = false;
    }

    Channel ChannelManager::find() const {

        Channel channel = NoChannel;

        // find free channel
        if ((mOccupiedChannels & Alpha) == 0) {
            channel = Alpha;
        }  else if (GLEW_ARB_texture_env_dot3) {
            if ((mOccupiedChannels & Red) == 0)   {
                channel = Red;
            } else if ((mOccupiedChannels & Green) == 0) {
                channel = Green;
            } else if ((mOccupiedChannels & Blue) == 0)  {
                channel = Blue;
            }
        }

        return channel;
    }

    Channel ChannelManager::request() {
        if (!mInOffscreenBuffer) {
            gOffscreenBuffer->BeginCapture();
            if (gOffscreenBuffer->haveSeparateContext()) {
                glFrontFace(FaceOrientation);
            }

            mInOffscreenBuffer = true;

            mCurrentChannel = NoChannel;
            mOccupiedChannels = NoChannel;
        }

        if (gOffscreenBuffer->haveSeparateContext()) {
            glViewport(OpenGL::canvasPos[0], OpenGL::canvasPos[1], OpenGL::canvasPos[2], OpenGL::canvasPos[3]);
            glMatrixMode(GL_PROJECTION);
            glLoadMatrixf(OpenGL::projection);
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(OpenGL::modelview);
        }

        mCurrentChannel = find();
        mOccupiedChannels |= mCurrentChannel;
        return mCurrentChannel;
    }

    Channel ChannelManager::current() const {
        return mCurrentChannel;
    }

    std::vector<Channel> ChannelManager::occupied() const {

        std::vector<Channel> result;
        result.reserve(4);

        if ((mOccupiedChannels & Alpha) != 0) {
            result.push_back(Alpha);
        }        
        if ((mOccupiedChannels & Red) != 0) {
            result.push_back(Red);
        }        
        if ((mOccupiedChannels & Green) != 0) {
            result.push_back(Green);
        }        
        if ((mOccupiedChannels & Blue) != 0) {
            result.push_back(Blue);
        }
     
        return result;
    }

    void ChannelManager::free() {
        if (mInOffscreenBuffer) {
            gOffscreenBuffer->EndCapture();
            mInOffscreenBuffer = false;
        }

        merge();
    }

    void ChannelManager::renderToChannel(bool on) {

        if (on) {
            switch (mCurrentChannel) {
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

        gOffscreenBuffer->Bind();
        gOffscreenBuffer->EnableTextureTarget();

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
        if (gOffscreenBuffer->GetTextureTarget() == GL_TEXTURE_2D) {
            // with ordinary pow-of-two texture coordinates are between 0 and 1
            // but we must assure only the used part of the texture is taken.
            factorX /= static_cast<float>(gOffscreenBuffer->GetWidth());
            factorY /= static_cast<float>(gOffscreenBuffer->GetHeight());
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
        if (!gOffscreenBuffer->haveSeparateContext()) {
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            glDisable(GL_TEXTURE_GEN_R);
            glDisable(GL_TEXTURE_GEN_Q);
        }

        glMatrixMode(GL_TEXTURE);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        gOffscreenBuffer->DisableTextureTarget();
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
        mPrimitives(std::vector<std::pair<std::vector<Primitive*>, int> >(Blue + 1)) {
    }

    void ChannelManagerForBatches::store(Channel channel, const std::vector<Primitive*>& primitives, int layer) {
        mPrimitives[channel] = std::make_pair(primitives, layer);
    }

    const std::vector<Primitive*> ChannelManagerForBatches::getPrimitives(Channel channel) const {
        return mPrimitives[channel].first;
    }    
    
    int ChannelManagerForBatches::getLayer(Channel channel) const {
        return mPrimitives[channel].second;
    }

    void ChannelManagerForBatches::clear() {
        mPrimitives = std::vector<std::pair<std::vector<Primitive*>, int> >(Blue + 1);
    }

} // namespace OpenCSG

