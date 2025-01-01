// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2002-2024, Florian Kirsch,
// Hasso-Plattner-Institute at the University of Potsdam, Germany
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
// renderGoldfeather.cpp
//
// stuff specific for the Goldfeather algorithm
//

#include "opencsgConfig.h"
#include <opencsg.h>
#include "opencsgRender.h"
#include "batch.h"
#include "channelManager.h"
#include "occlusionQuery.h"
#include "openglHelper.h"
#include "primitiveHelper.h"
#include "scissorMemo.h"
#include <algorithm>

namespace OpenCSG {

    namespace {

        ScissorMemo* scissor;

        class GoldfeatherChannelManager : public ChannelManagerForBatches {
        public:
            virtual void merge();
        };

        void GoldfeatherChannelManager::merge() {

            bool isFixedFunction = true;
            setupProjectiveTexture(isFixedFunction);

            glEnable(GL_ALPHA_TEST);
            glAlphaFunc(GL_GEQUAL, 0.5f); // accuracy issue with GL_EQUAL 1.0 on FX5600
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

            std::vector<Channel> channels = occupied();
            for (std::vector<Channel>::const_iterator c = channels.begin(); c!=channels.end(); ++c) {

                const std::vector<Primitive*> primitives = getPrimitives(*c);

                scissor->recall(*c);
                scissor->enableScissor();

                setupTexEnv(*c);

                if (getLayer(*c) == -1) {
                
                    glEnable(GL_CULL_FACE);
                    for (Batch::const_iterator j = primitives.begin(); j != primitives.end(); ++j) {
                        glCullFace((*j)->getOperation() == Intersection ? GL_BACK : GL_FRONT);
                        (*j)->render();
                    }
                } else {
                    // shapes of interest: we need to determine the appropriate layer of 
                    // the shapes, using stencil counting
                    glClearStencil(0);
                    glStencilMask(OpenGL::stencilMask);
                    glClear(GL_STENCIL_BUFFER_BIT);
                    OpenGL::renderLayer(getLayer(*c), primitives);
                    glDisable(GL_STENCIL_TEST);
                }
            }

            glDisable(GL_ALPHA_TEST);
            glDisable(GL_CULL_FACE);
            glDepthFunc(GL_LEQUAL);

            scissor->disableScissor();

            resetProjectiveTexture(isFixedFunction);

            clear();
        }

        GoldfeatherChannelManager* channelMgr;

        void touchFragments(const Batch& batch) {
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            GLboolean origVertexArrayState = glIsEnabled(GL_VERTEX_ARRAY);
            if (!origVertexArrayState) {
                glEnableClientState(GL_VERTEX_ARRAY);
            }

            for (Batch::const_iterator j = batch.begin(); j != batch.end(); ++j) {

                float fminx, fminy, fminz, fmaxx, fmaxy, fmaxz;
                (*j)->getBoundingBox(fminx, fminy, fminz, fmaxx, fmaxy, fmaxz);

                const GLfloat v[8] = {
                    fminx, fminy,
                    fmaxx, fminy,
                    fminx, fmaxy,
                    fmaxx, fmaxy
                };

                glVertexPointer(2, GL_FLOAT, 0, v);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }

            if (!origVertexArrayState) {
                glDisableClientState(GL_VERTEX_ARRAY);
            }

            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
        }

        GLenum getParityDepthFunc()
        {
            int optimizationSetting = getOption(ParityZPassOptimization);
            if (optimizationSetting == OptimizationOn ||
                optimizationSetting == OptimizationForceOn)
                return GL_LEQUAL;
            else
                return GL_GREATER;
        }

        void discardFragments(const Batch& batch, int parity, int mask) {
            glDepthFunc(GL_ALWAYS);
            glDepthMask(GL_TRUE);
            glDepthRange(1.0, 1.0);
            glStencilMask(mask);
            glStencilFunc(GL_NOTEQUAL, parity, mask);
            glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
            channelMgr->renderToChannel(true);
            glColor4ub(0, 0, 0, 0);

            touchFragments(batch);

            glDepthRange(0.0, 1.0);
            glDepthMask(GL_FALSE);
            glDepthFunc(getParityDepthFunc());
        }

        void discardFragments(int parity, int mask) {
            glDepthFunc(GL_ALWAYS);
            glDepthMask(GL_TRUE);
            glDepthRange(1.0, 1.0);
            glStencilMask(mask);
            glStencilFunc(GL_NOTEQUAL, parity, mask);
            glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
            channelMgr->renderToChannel(true);
            glColor4ub(0, 0, 0, 0);

            OpenGL::drawQuad();

            glDepthRange(0.0, 1.0);
            glDepthMask(GL_FALSE);
            glDepthFunc(getParityDepthFunc());
        }

        void discardFragments(
                const Batch& batch,
                int parity, int mask,
                const ScissorMemo* scissor,
                bool layered) {

            if (layered) {
                discardFragments(parity, mask);
            } else {
                if (scissor->isDepthBoundsTestEnabled())
                    glDisable(GL_DEPTH_BOUNDS_TEST_EXT);
                discardFragments(batch, parity, mask);
                if (scissor->isDepthBoundsTestEnabled())
                    glEnable(GL_DEPTH_BOUNDS_TEST_EXT);
            }
        }

        void parityTestAndDiscard(
                const Batch& shapesOfInterest,
                const std::vector<Primitive*>& primitives,
                const ScissorMemo* scissor,
                bool layered,
                unsigned int stencilMax) {

            glDepthMask(GL_FALSE);
            glDepthFunc(getParityDepthFunc());

            glEnable(GL_STENCIL_TEST);

            unsigned int parityValue = 1;
            unsigned int allParityTestValues = 0;

            // for all shapes of the intersection, we conduct the parity test.
            // for fragments for which it fails, we mark them as not visible
            // (parity testing means to check whether the number of surfaces in front
            // of the current z-buffer is even or uneven)
            for (std::vector<Primitive*>::const_iterator itr = primitives.begin(); itr != primitives.end(); ++itr ) {
                if (!layered) {

                    // substracted shapes that form a part of the shapes of interest
                    // can not alter the visibility of the other shapes of interest. the reason:
                    // for other substracted shapes: by definition
                    // for other intersecting shapes: since the bounding boxes do no overlap
                    //
                    // for intersecting shapes, the above only is true if it is the only shape 
                    // in the list of shapes of interest
                    Batch::const_iterator res = 
                        std::find(shapesOfInterest.begin(), shapesOfInterest.end(), *itr);
                    bool isContained = (res != shapesOfInterest.end());

                    if (isContained && ((*itr)->getOperation() == Subtraction || shapesOfInterest.size() == 1)) {
                        continue;
                    }

                    // for substracted shapes that don't touch the shapes of interest,
                    // the parity test would always fail. thus, they are omited here. 
                    bool needParityTest = ((*itr)->getOperation() == Intersection);
                    if (!needParityTest) {
                        for (Batch::const_iterator k = shapesOfInterest.begin(); k != shapesOfInterest.end(); ++k) {
                            if (Algo::intersectXYZ(*itr, *k)) {
                                needParityTest = true; 
                                break;
                            }
                        }
                    }
                    if (!needParityTest)
                        continue;
                }

                // we only need one bit in the stencil buffer for each parity test.
                // Thus we iterate over all bits, and only when all bits have been
                // used, we discard fragments marked invisible by the parity test,
                // and clear the stencil buffer.
                if (parityValue >= stencilMax) {
                    discardFragments(shapesOfInterest, allParityTestValues, parityValue - 1, scissor, layered);
                    parityValue = 1;
                    allParityTestValues = 0;
                }

                // parity test: count surfaces in front of shapes of interest
                channelMgr->renderToChannel(false);
                glStencilFunc(GL_ALWAYS, 0, parityValue);
                glStencilMask(parityValue);
                glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
                (*itr)->render();

                // after the parity test, the reaction differs whether we have a
                // intersecting or a subtracted shape:
                //   if #(surfaces) is even (stencilValue == 0):
                //      if intersecting shapes      -> shape do not intersect, set depth to max
                //      if subtracting second shape -> first shape is visible, don't change depth
                //   if #(surfaces) is uneven (stencilValue == parityValue):
                //      if intersecting shapes      -> shapes intersect, first shape is visible, don't change depth
                //      if subtracting second shape -> front surface of first shape is subtracted, not visible. set depth to max
                //if (!(*i)->complement_) {
                if ((*itr)->getOperation() == Intersection) {
                    allParityTestValues += parityValue;
                }

                parityValue <<= 1;
            }

            // discard fragments marked invisible the last parity tests (and clear the stencil buffer)
            if (parityValue != 1) { // that would mean no parity test had occured at all
                discardFragments(shapesOfInterest, allParityTestValues, parityValue - 1, scissor, layered);
            }

            glDisable(GL_STENCIL_TEST);
        }

    } // unnamed namespace

    void renderGoldfeather(const std::vector<Primitive*>& primitives) 
    {
        scissor = new ScissorMemo;

        Batcher batches(primitives);

        scissor->setIntersected(primitives);

        for (std::vector<Batch>::const_iterator itr = batches.begin(); itr != batches.end(); ++itr) {
            unsigned int maxConvexity = Algo::getConvexity(*itr);
            for (unsigned int currentLayer = 0; currentLayer < maxConvexity; ++currentLayer) {
                
                if (channelMgr->request() == NoChannel) {
                    channelMgr->free();
                    channelMgr->request();
                }

                channelMgr->renderToChannel(true); glColor4ub(255, 255, 255, 255);
                glStencilMask(OpenGL::stencilMask);
                glEnable(GL_STENCIL_TEST);

                glDepthFunc(GL_ALWAYS);
                glDepthMask(GL_TRUE);

                scissor->setCurrent(*itr);
                scissor->store(channelMgr->current());
                scissor->enableScissor();

                if (maxConvexity == 1) {
                    // shapes of interest: we need to determine which parts of them are visible.
                    // first assume they are fully visible
                    glStencilFunc(GL_ALWAYS, 0, OpenGL::stencilMask);
                    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
                    glEnable(GL_CULL_FACE);

                    for (Batch::const_iterator j = itr->begin(); j != itr->end(); ++j) {
                        glCullFace((*j)->getOperation() == Intersection ? GL_BACK : GL_FRONT);
                        (*j)->render();
                    }

                    glDisable(GL_CULL_FACE);
                } else {
                    // shapes of interest: we need to determine the appropriate layer of 
                    // the shapes, using stencil counting
                    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                    OpenGL::renderLayer(currentLayer, *itr);
                    glClear(GL_STENCIL_BUFFER_BIT);
                }

                GLenum depthFunc = getParityDepthFunc();
                if (depthFunc == GL_GREATER)
                    scissor->enableDepthBoundsFront();
                else if (depthFunc == GL_LEQUAL)
                    scissor->enableDepthBoundsBack();

                parityTestAndDiscard(*itr, primitives, scissor, false, OpenGL::stencilMask);

                scissor->disableDepthBounds();
                scissor->disableScissor();

                channelMgr->store(channelMgr->current(), *itr, maxConvexity == 1 ? -1 : static_cast<int>(currentLayer));
            }
        }

        channelMgr->free();

        delete scissor;
    }

    bool renderOcclusionQueryGoldfeather(const std::vector<Primitive*>& primitives)
    {
        scissor = new ScissorMemo;

        unsigned int layer = 0;

        scissor->setIntersected(primitives);
        scissor->setCurrent(primitives);

        OpenGL::OcclusionQuery* occlusionTest = 0;

        bool retVal = true;

        while (true) {
            if (channelMgr->request() == NoChannel) {
                channelMgr->free();
                channelMgr->request();
            }

            scissor->store(channelMgr->current());
            scissor->enableScissor();

            if (!occlusionTest) {
                occlusionTest = OpenGL::getOcclusionQuery(false);
                if (!occlusionTest) {
                    retVal = false;
                    break;
                }
            }

            channelMgr->renderToChannel(true);

            glStencilMask(OpenGL::stencilMask);
            glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            glDepthFunc(GL_ALWAYS);
            glDepthMask(GL_TRUE);
            glColor4ub(255, 255, 255, 255);

            occlusionTest->beginQuery();
            OpenGL::renderLayer(layer, primitives);
            occlusionTest->endQuery();
            // the fragment count query could occur here, but benches show that
            // the algorithm is faster if the query is delayed.
            glClear(GL_STENCIL_BUFFER_BIT);

            parityTestAndDiscard(primitives, primitives, scissor, true, OpenGL::stencilMax);

            unsigned int anyFragmentRendered = occlusionTest->getQueryResult();
            if (!anyFragmentRendered) {
                retVal = true;
                break;
            }

            channelMgr->store(channelMgr->current(), primitives, layer);

            scissor->disableScissor();

            ++layer;

            // There are more layers than the stencil buffer allows us to
            // render. Return claiming success (which is cheating, since
            // we were not really successful)
            if (layer == OpenGL::stencilMax) {
                retVal = true;
                break;
            }
        }

        delete occlusionTest;

        channelMgr->free();

        delete scissor;

        return retVal;
    }

    void renderDepthComplexitySamplingGoldfeather(const std::vector<Primitive*>& primitives) 
    {
        scissor = new ScissorMemo;

        scissor->setIntersected(primitives);
        scissor->setCurrent(primitives);
        scissor->enableScissor();

        unsigned int depthComplexity = OpenGL::calcMaxDepthComplexity(primitives, scissor->getIntersectedArea());

        scissor->disableScissor();

        for (unsigned int layer = 0; layer < depthComplexity; ++layer) {
            if (channelMgr->request() == NoChannel) {
                channelMgr->free();
                channelMgr->request();
            }

            scissor->store(channelMgr->current());
            scissor->enableScissor();

            channelMgr->renderToChannel(true);

            glStencilMask(OpenGL::stencilMask);
            glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            glDepthFunc(GL_ALWAYS);
            glDepthMask(GL_TRUE);
            glColor4ub(255, 255, 255, 255);
            OpenGL::renderLayer(layer, primitives);

            glClear(GL_STENCIL_BUFFER_BIT);

            parityTestAndDiscard(primitives, primitives, scissor, true, OpenGL::stencilMax);

            channelMgr->store(channelMgr->current(), primitives, layer);

            scissor->disableScissor();
        }

        channelMgr->free();

        delete scissor;
    }

    void renderGoldfeather(const std::vector<Primitive*>& primitives, DepthComplexityAlgorithm algorithm)
    {
        channelMgr = new GoldfeatherChannelManager;
        if (channelMgr->init())
        {
            switch (algorithm) {
            case OcclusionQuery:
                if (renderOcclusionQueryGoldfeather(primitives))
                    break;  // success
                // coming here should not happen in practice due to the check
                // for extensions performed by the caller. Anyway, if it happens,
                // fall through
            case NoDepthComplexitySampling:
                renderGoldfeather(primitives);
                break;
            case DepthComplexitySampling:
                renderDepthComplexitySamplingGoldfeather(primitives);
                break;
            case DepthComplexityAlgorithmUnused:
                break; // does not happen
            }
        }

        delete channelMgr;
    }

} // namespace OpenCSG
