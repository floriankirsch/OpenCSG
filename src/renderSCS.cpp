// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2002-2016, Florian Kirsch,
// Hasso-Plattner-Institute at the University of Potsdam, Germany
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
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

//
// renderSCS.cpp
//
// stuff specific for the SCS algorithm
//

#include "opencsgConfig.h"
#include <opencsg.h>
#include "opencsgRender.h"
#include "batch.h"
#include "channelManager.h"
#include "context.h"
#include "occlusionQuery.h"
#include "openglHelper.h"
#include "primitiveHelper.h"
#include "scissorMemo.h"

#include <algorithm>
#include <map>

namespace OpenCSG {

    namespace {

        ScissorMemo* scissor;

        struct IdBufferId {
            GLubyte r;
            GLubyte g;
            GLubyte b;
            GLubyte a;
            GLubyte* vec() {
                return &r;
            }
        };

        struct RenderData {
            IdBufferId bufferId;
        };

        std::map<Primitive*, RenderData> gRenderInfo;

        RenderData* getRenderData(Primitive* primitive) {
            RenderData* dta = &(gRenderInfo.find(primitive))->second;
            return dta;
        }

        // Stores Ids in the alpha buffer only -> only 255 primitives are possible
        class SCSChannelManagerAlphaOnly : public ChannelManagerForBatches {
        public:
            virtual void merge();
        };

        void SCSChannelManagerAlphaOnly::merge() {

            bool isFixedFunction = true;
            setupProjectiveTexture(isFixedFunction);

            glEnable(GL_ALPHA_TEST);
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

            std::vector<Channel> channels = occupied();
            for (std::vector<Channel>::const_iterator c = channels.begin(); c!=channels.end(); ++c) {

                setupTexEnv(*c);
                scissor->recall(*c);
                scissor->enableScissor();

                const std::vector<Primitive*> primitives = getPrimitives(*c);
                for (std::vector<Primitive*>::const_iterator j = primitives.begin(); j != primitives.end(); ++j) {
                    glCullFace((*j)->getOperation() == Intersection ? GL_BACK : GL_FRONT);
                    RenderData* primitiveData = getRenderData(*j);
                    GLubyte id = primitiveData->bufferId.a;

                    // Here is an interesting bug, which happened on an ATI HD4670, but actually
                    // might happen on every hardware. I am not sure whether it can be solved 
                    // correctly.

                    // Problem is that in optimized mode with some compilers (VC6, Visual Studio 2003
                    // in particular), when setting the alpha func as follows:
                    // glAlphaFunc(GL_EQUAL, static_cast<float>(id) / 255.0f);
                    // the division is optimized as multiplication with 1.0f/255.0f.
                    // This is a fine and valid optimization. Unfortunately, the results
                    // are not exactly the same as the direct division for some ids.
                    // Which is apparently what the ATI driver is doing internally.
                    // So with comparison with GL_EQUAL fails. 

                    // Fortunately the OpenGL standard enforces that the mapping of color byte
                    // values to float fragment values be done by division. So if the
                    // solution found below (just working at double precision) proves
                    // to work once, it should work forever, such that a precompiling
                    // lookup table containing the correct alpha reference values is 
                    // not required. However a bad feeling remains.

                    // The SCSChannelManagerFragmentProgram path implemented below should fix this.

                    double alpha = static_cast<double>(id) / 255.0;
                    GLfloat fAlpha = static_cast<float>(alpha);
                    glAlphaFunc(GL_EQUAL, fAlpha);

                    (*j)->render();
                }
            }

            scissor->disableScissor();

            glDisable(GL_ALPHA_TEST);
            glDisable(GL_CULL_FACE);
            glDepthFunc(GL_LEQUAL);

            resetProjectiveTexture(isFixedFunction);

            clear();
        }

        // Stores Ids in the all components of the color buffer
        // -> in theory, 2^32-1 primitives are possible
        class SCSChannelManagerFragmentProgram : public ChannelManagerForBatches {
        public:
            virtual Channel request();
            virtual void merge();
        };

        Channel SCSChannelManagerFragmentProgram::request() {
            ChannelManagerForBatches::request();
            mCurrentChannel = AllChannels;
            mOccupiedChannels = mCurrentChannel;
            return mCurrentChannel;
        }

        // Emulates eye texgen
        static const char mergeVertexProgram[] =
"!!ARBvp1.0 OPTION ARB_position_invariant;\n"
"ATTRIB  pos = vertex.position;\n"
"ATTRIB  col = vertex.color;\n"
"OUTPUT  outCol = result.color;\n"
"OUTPUT  outTex0 = result.texcoord[0];\n"
"PARAM   mvpmat[4] = { state.matrix.mvp };\n"
"PARAM   texmat[4] = { state.matrix.texture[0] };\n"
"TEMP    eye;\n"
"TEMP    tex;\n"
"DP4     eye.x, mvpmat[0], pos;\n"
"DP4     eye.y, mvpmat[1], pos;\n"
"DP4     eye.z, mvpmat[2], pos;\n"
"DP4     eye.w, mvpmat[3], pos;\n"
"DP4     tex.x, texmat[0], eye;\n"
"DP4     tex.y, texmat[1], eye;\n"
"DP4     tex.z, texmat[2], eye;\n"
"DP4     tex.w, texmat[3], eye;\n"
"MOV     outTex0, tex;\n"
"MOV     outCol, col;\n"
"END";

        // Subtract color from texture value, takes the absolute value
        // and adds all components into each channel of the result, scaled by 2.0f.
        // This way, all 32-bits of the color channel can be used
        // for an 'equal' alpha test, i.e, to check if value in texture
        // and color are equal.

        // Note that 1.0f/255.0f cannot be the result of the above computation.
        // Either the result is 0 (if all components were equal, disregarding
        // numerical errors), or larger/equal than 2.0f/255.0f. The alpha function
        // then is actually set to  GL_LESS, 1.0f/255.0f. This is robust,
        // in contract to the GL_EQUAL in SCSChannelManagerAlphaOnly above.
        // The scaling by 2.0f is required for NVidia hardware, which considers
        // the alpha function GL_LESS, 0.5f/255.0f as GL_LESS, 0.0f for some reason.
        static const char mergeFragmentProgramRect[] =
"!!ARBfp1.0\n"
"TEMP    temp;\n"
"ATTRIB  tex0 = fragment.texcoord[0];\n"
"ATTRIB  col0 = fragment.color;\n"
"PARAM   scaleByTwo = { 2.0, 2.0, 2.0, 2.0 };\n"
"OUTPUT  out = result.color;\n"
"TXP     temp, tex0, texture[0], RECT;\n"
"SUB     temp, temp, col0;\n"
"ABS     temp, temp;\n"
"DP4     out, temp, scaleByTwo;\n"
"END";

        static const char mergeFragmentProgram2D[] =
"!!ARBfp1.0\n"
"TEMP    temp;\n"
"ATTRIB  tex0 = fragment.texcoord[0];\n"
"ATTRIB  col0 = fragment.color;\n"
"PARAM   scaleByTwo = { 2.0, 2.0, 2.0, 2.0 };\n"
"OUTPUT  out = result.color;\n"
"TXP     temp, tex0, texture[0], 2D;\n"
"SUB     temp, temp, col0;\n"
"ABS     temp, temp;\n"
"DP4     out, temp, scaleByTwo;\n"
"END";

        void SCSChannelManagerFragmentProgram::merge()
        {
            GLuint vId = OpenGL::getARBVertexProgram(mergeVertexProgram, (sizeof(mergeVertexProgram) / sizeof(mergeVertexProgram[0])) - 1);
            glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vId);
            glEnable(GL_VERTEX_PROGRAM_ARB);

            GLuint fId =
                 isRectangularTexture()
                   ? OpenGL::getARBFragmentProgram(mergeFragmentProgramRect, (sizeof(mergeFragmentProgramRect) / sizeof(mergeFragmentProgramRect[0])) - 1)
                   : OpenGL::getARBFragmentProgram(mergeFragmentProgram2D, (sizeof(mergeFragmentProgram2D) / sizeof(mergeFragmentProgram2D[0])) - 1);
            glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fId);
            glEnable(GL_FRAGMENT_PROGRAM_ARB);

            bool isFixedFunction = false;
            setupProjectiveTexture(isFixedFunction);

            glEnable(GL_ALPHA_TEST);
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

            glAlphaFunc(GL_LESS, 1.0f / 255.0f);

            std::vector<Channel> channels;
            channels.push_back(AllChannels);
            for (std::vector<Channel>::const_iterator c = channels.begin(); c!=channels.end(); ++c) {

                scissor->recall(*c);
                scissor->enableScissor();

                const std::vector<Primitive*> primitives = getPrimitives(*c);
                for (std::vector<Primitive*>::const_iterator j = primitives.begin(); j != primitives.end(); ++j) {
                    glCullFace((*j)->getOperation() == Intersection ? GL_BACK : GL_FRONT);
                    RenderData* primitiveData = getRenderData(*j);
                    GLubyte * id = primitiveData->bufferId.vec();
                    glColor4ubv(id);
                    (*j)->render();
                }
            }

            scissor->disableScissor();

            glDisable(GL_ALPHA_TEST);
            glDisable(GL_CULL_FACE);
            glDepthFunc(GL_LEQUAL);
            glDisable(GL_FRAGMENT_PROGRAM_ARB);
            glDisable(GL_VERTEX_PROGRAM_ARB);

            resetProjectiveTexture(isFixedFunction);

            clear();
        }


        ChannelManagerForBatches* getChannelManager() {

            if (GLEW_ARB_vertex_program && GLEW_ARB_fragment_program) {
                return new SCSChannelManagerFragmentProgram;
            }

            return new SCSChannelManagerAlphaOnly;
        }


        class IDGenerator {
        public:
            IDGenerator() : counter(0) {};
            IdBufferId newID() {
                ++counter;
                IdBufferId newId;
                newId.r =  (counter >> 24) & 0xff;
                newId.g =  (counter >> 16) & 0xff;
                newId.b =  (counter >>  8) & 0xff;
                newId.a =  (counter >>  0) & 0xff;
                return newId;
            };

        private:
            unsigned int counter;
        };

        ChannelManagerForBatches* channelMgr;

        void renderIntersectedFront(const std::vector<Primitive*>& primitives) {

            const int numberOfPrimitives = primitives.size();

            glDepthMask(GL_TRUE);

            // optimization for only one shape
            if (numberOfPrimitives == 1) {
                channelMgr->renderToChannel(true);
                glDepthFunc(GL_GREATER);
                glCullFace(GL_BACK);
                glEnable(GL_CULL_FACE);
                RenderData* primitiveData = getRenderData(primitives[0]);
                GLubyte * id = primitiveData->bufferId.vec();
                glColor4ubv(id);
                primitives[0]->render();
                glDisable(GL_CULL_FACE);
                glDepthFunc(GL_LESS);

                return;
            }

            // draw furthest front face
            channelMgr->renderToChannel(true);
            glStencilMask(OpenGL::stencilMask);
            glDepthFunc(GL_GREATER);
            glCullFace(GL_BACK);
            glEnable(GL_CULL_FACE);

            {
                for (std::vector<Primitive*>::const_iterator i = primitives.begin(); i != primitives.end(); ++i) {
                    RenderData* primitiveData = getRenderData(*i);
                    GLubyte * id = primitiveData->bufferId.vec();
                    glColor4ubv(id);
                    (*i)->render();
                }
            }

            // count back faces behind furthest front face
            channelMgr->renderToChannel(false);
            glStencilFunc(GL_ALWAYS, 0, OpenGL::stencilMask);
            glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
            glEnable(GL_STENCIL_TEST);
            glDepthMask(GL_FALSE);
            glCullFace(GL_FRONT);

            {
                for (std::vector<Primitive*>::const_iterator i = primitives.begin(); i != primitives.end(); ++i) {
                    (*i)->render();
                }
            }

            // where #back faces behind furthest front face != #intersected shapes
            // ->reset fragment
            channelMgr->renderToChannel(true);
            glStencilFunc(GL_NOTEQUAL, numberOfPrimitives, OpenGL::stencilMask);
            glDepthFunc(GL_ALWAYS);
            glDepthRange(0.0, 0.0);
            glDepthMask(GL_TRUE);
            glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
            glDisable(GL_CULL_FACE);
            glColor4ub(0, 0, 0, 0);

            OpenGL::drawQuad();

            glDepthRange(0.0, 1.0);
            glDepthFunc(GL_LESS);
            glDisable(GL_STENCIL_TEST);
        }

        void subtractPrimitives(std::vector<Batch>::const_iterator begin, 
                                std::vector<Batch>::const_iterator end,
                                const unsigned int iterations) {

            if (begin == end) {
                return;
            }

            glStencilMask(OpenGL::stencilMask);
            glEnable(GL_STENCIL_TEST);
            glEnable(GL_CULL_FACE);

            unsigned int stencilref = 0;
            int sense = 1;
            unsigned int changes = 0;
            std::vector<Batch>::const_iterator i = begin;
            do {
                // create a distinct reference value
                ++stencilref;
                if (stencilref == OpenGL::stencilMax) {
                    glClear(GL_STENCIL_BUFFER_BIT);
                    stencilref = 1;
                }

                channelMgr->renderToChannel(false);
                glDepthFunc(GL_LESS);
                glDepthMask(GL_FALSE);
                glStencilFunc(GL_ALWAYS, stencilref, OpenGL::stencilMask);
                glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
                glCullFace(GL_BACK);

                { 
                    for (Batch::const_iterator j = i->begin(); j != i->end(); ++j) {
                        (*j)->render();
                    }
                }

                // where front faces have been visible, render back faces
                channelMgr->renderToChannel(true);
                glDepthFunc(GL_GREATER);
                glDepthMask(GL_TRUE);
                glCullFace(GL_FRONT);
                glStencilFunc(GL_EQUAL, stencilref, OpenGL::stencilMask);
                glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);

                {
                    for (Batch::const_iterator j = i->begin(); j != i->end(); ++j) {
                        RenderData* primitiveData = getRenderData(*j);
                        GLubyte * id = primitiveData->bufferId.vec();
                        glColor4ubv(id);
                        (*j)->render();
                    }
                }

                if (sense == 1) {
                    ++i; 
                    if (i == end) {
                        sense = 0;
                        --i; if (i == begin) break; // only one subtracted shape
                        --i;
                        ++changes;
                    }
                } else {
                    if (i == begin) {
                        sense = 1;
                        ++i;
                        ++changes;
                    } else {
                        --i;
                    }
                }
            } while (changes < iterations);

            glDisable(GL_STENCIL_TEST);
        }

        bool subtractPrimitivesWithOcclusionQueries(std::vector<Batch>::const_iterator begin,
                                                    std::vector<Batch>::const_iterator end) {

            const unsigned int numberOfBatches = end - begin;
            if (numberOfBatches == 0) {
                return true;
            }

            OpenGL::OcclusionQuery* occlusionTest = OpenGL::getOcclusionQuery(true);
            if (!occlusionTest) {
                return false;
            }

            glStencilMask(OpenGL::stencilMask);
            glEnable(GL_STENCIL_TEST);
            glEnable(GL_CULL_FACE);

            std::vector<unsigned int> fragmentcount(numberOfBatches, 0);

            unsigned int stencilref = 0;
            std::vector<Batch>::const_iterator i = begin;
            unsigned int shapesWithoutUpdate = 0;    
            unsigned int shapeCount = 0;
            int idx=0;

            do {
                // create a distinct reference value
                ++stencilref;
                if (stencilref == OpenGL::stencilMax) {
                    glClear(GL_STENCIL_BUFFER_BIT);
                    stencilref = 1;
                }

                channelMgr->renderToChannel(false);
                glDepthFunc(GL_LESS);
                glDepthMask(GL_FALSE);
                glStencilFunc(GL_ALWAYS, stencilref, OpenGL::stencilMask);
                glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
                glCullFace(GL_BACK);

                occlusionTest->beginQuery();
                { 
                    for (Batch::const_iterator j = i->begin(); j != i->end(); ++j) {
                        (*j)->render();
                    }
                }
                occlusionTest->endQuery();
                // the fragment count query could occur here, but benches show that
                // the algorithm is faster if the query is delayed.
                // where front faces have been visible, render back faces
                channelMgr->renderToChannel(true);
                glDepthFunc(GL_GREATER);
                glDepthMask(GL_TRUE);
                glCullFace(GL_FRONT);
                glStencilFunc(GL_EQUAL, stencilref, OpenGL::stencilMask);
                glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);

                {
                    for (Batch::const_iterator j = i->begin(); j != i->end(); ++j) {
                        RenderData* primitiveData = getRenderData(*j);
                        GLubyte * id = primitiveData->bufferId.vec();
                        glColor4ubv(id);
                        (*j)->render();
                    }
                }

                unsigned int newFragmentCount = occlusionTest->getQueryResult();
                if (newFragmentCount != fragmentcount[idx]) {
                    fragmentcount[idx] = newFragmentCount;
                    shapesWithoutUpdate = 0;
                } else {
                    ++shapesWithoutUpdate;
                }

                ++i; ++idx; if (i == end) {
                    i = begin;
                    idx = 0;
                }
    
                ++shapeCount;
                if (shapeCount >= (numberOfBatches) * (numberOfBatches) - (numberOfBatches) + 1) break;

            } while (shapesWithoutUpdate < numberOfBatches);

            delete occlusionTest;

            glDisable(GL_STENCIL_TEST);

            return true;
        }

        void renderIntersectedBack(const std::vector<Primitive*>& primitives) {
            // where a back face of intersected shape is in front of any subtracted shape
            // mask fragment as invisible. Updating depth values is not necessary, so when
            // having IDs, this is kind of simple. 
            channelMgr->renderToChannel(true);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_LESS);
            glColor4ub(0, 0, 0, 0);

            for (std::vector<Primitive*>::const_iterator i = primitives.begin(); i != primitives.end(); ++i) {
                (*i)->render();
            }

            glDepthMask(GL_TRUE);
        }

    } // unnamed namespace

    void renderSCS(const std::vector<Primitive*>& primitives, DepthComplexityAlgorithm algorithm) {

        channelMgr = getChannelManager();
        if (!channelMgr->init())
        {
            delete channelMgr;
            return;
        }

        gRenderInfo.clear();

        scissor = new ScissorMemo;

        std::vector<Primitive*> intersected; intersected.reserve(primitives.size());
        std::vector<Primitive*> subtracted;  subtracted.reserve(primitives.size());

        {
            IDGenerator IDMaker;
            for (std::vector<Primitive*>::const_iterator itr = primitives.begin(); itr != primitives.end(); ++itr) {
                {
                    RenderData dta; 
                    dta.bufferId = IDMaker.newID();
                    gRenderInfo.insert(std::make_pair(*itr, dta));
                }
                Operation operation= (*itr)->getOperation();
                if (operation == Intersection) {
                    intersected.push_back(*itr);
                } else if (operation == Subtraction) {
                    subtracted.push_back(*itr);
                }
            }
        }

        Batcher subtractedBatches(subtracted);

        scissor->setIntersected(intersected);
        scissor->setCurrent(intersected);

        unsigned int depthComplexity = 0;
        if (algorithm == DepthComplexitySampling) {
            scissor->enableScissor();
            glClear(GL_STENCIL_BUFFER_BIT);
            depthComplexity = 
                (std::min)(OpenGL::calcMaxDepthComplexity(subtracted, scissor->getCurrentArea()), 
                         subtractedBatches.size());
        }

        channelMgr->request();
        channelMgr->renderToChannel(true);

        scissor->enableScissor();
        scissor->store(channelMgr->current());

        glDepthMask(GL_TRUE);
        glStencilMask(OpenGL::stencilMask);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glClearDepth(0.0);      // near clipping plane! essential for algorithm!
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glClearDepth(1.0);

        renderIntersectedFront(intersected);
        scissor->enableDepthBounds();
        switch (algorithm) {
        case OcclusionQuery:
            if (subtractPrimitivesWithOcclusionQueries(subtractedBatches.begin(), subtractedBatches.end()))
                break; // success
                       // else fall through (should we just give up here?)
        case NoDepthComplexitySampling:
            subtractPrimitives(subtractedBatches.begin(), subtractedBatches.end(), subtractedBatches.size());
            break;
        case DepthComplexitySampling:
            subtractPrimitives(subtractedBatches.begin(), subtractedBatches.end(), depthComplexity);
            break;
        case DepthComplexityAlgorithmUnused:
            break; // does not happen when invoked correctly           
        }
        scissor->disableDepthBounds();
        renderIntersectedBack(intersected);

        scissor->disableScissor();

        channelMgr->store(channelMgr->current(), primitives, 0);
        channelMgr->free();

        delete scissor;
        delete channelMgr;
    }

} // namespace OpenCSG
