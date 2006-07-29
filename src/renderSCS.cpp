// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2002-2006, Florian Kirsch,
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
// renderSCS.cpp
//
// stuff specific for the SCS algorithm
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
#include <map>

namespace OpenCSG {

    namespace {

        ScissorMemo* scissor;

        struct RenderData {
            unsigned int stencilID_;
        };

        std::map<Primitive*, RenderData> renderInfo_;

        RenderData* getRenderData(Primitive* primitive) {
            RenderData* dta = &(renderInfo_.find(primitive))->second;
            return dta;
        }
    
        class SCSChannelManager : public ChannelManagerForBatches {
        public:
            virtual void merge();
        };

        void SCSChannelManager::merge() {

            setupProjectiveTexture();

            glEnable(GL_ALPHA_TEST);
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);

            std::vector<Channel> channels = occupied();
            for (std::vector<Channel>::const_iterator c = channels.begin(); c!=channels.end(); ++c) {

                setupTexEnv(*c);
                scissor->recall(*c);
                scissor->enable();

                const std::vector<Primitive*> primitives = getPrimitives(*c);
                for (std::vector<Primitive*>::const_iterator j = primitives.begin(); j != primitives.end(); ++j) {
                    glCullFace((*j)->getOperation() == Intersection ? GL_BACK : GL_FRONT);
                    RenderData* primitiveData = getRenderData(*j);
                    unsigned char id = primitiveData->stencilID_;
                    glAlphaFunc(GL_EQUAL, static_cast<float>(id) / 255.0f);
                    (*j)->render();
                }
            }

            scissor->disable();

            glDisable(GL_ALPHA_TEST);
            glDisable(GL_CULL_FACE);
            glDepthFunc(GL_LEQUAL);

            resetProjectiveTexture();

            clear();
        }

        class IDGenerator {
        public:
            IDGenerator() { currentID_ = 0; };
            unsigned int newID() { return ++currentID_; };

        private:
            unsigned int currentID_;
        };

        IDGenerator* IDGenerator_;
        SCSChannelManager* channelMgr;

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
                GLubyte b = primitiveData->stencilID_; glColor4ub(b, b, b, b);
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
                    GLubyte b = primitiveData->stencilID_; glColor4ub(b, b, b, b);
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

            int stencilref = 0;
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
                        GLubyte b = primitiveData->stencilID_; glColor4ub(b, b, b, b);
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

        void subtractPrimitivesWithOcclusionQueries(std::vector<Batch>::const_iterator begin, 
                                                    std::vector<Batch>::const_iterator end) {

            const unsigned int numberOfBatches = end - begin;
            if (numberOfBatches == 0) {
                return;
            }

            glStencilMask(OpenGL::stencilMask);
            glEnable(GL_STENCIL_TEST);
            glEnable(GL_CULL_FACE);

            OpenGL::OcclusionQuery* occlusionTest = OpenGL::getOcclusionQuery();

            std::vector<unsigned int> fragmentcount(numberOfBatches, 0);

            int stencilref = 0;
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
                        GLubyte b = primitiveData->stencilID_; glColor4ub(b, b, b, b);
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
        }

        void renderIntersectedBack(const std::vector<Primitive*>& primitives) {
            // where a back face of intersected shape is in front of any subtracted shape
            // mask fragment as invisible. Updating depth calues is not necessary, so when
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

        renderInfo_.clear();

        IDGenerator_ = new IDGenerator();

        channelMgr = new SCSChannelManager;
        scissor = new ScissorMemo;

        std::vector<Primitive*> intersected; intersected.reserve(primitives.size());
        std::vector<Primitive*> subtracted;  subtracted.reserve(primitives.size());

        {
            for (std::vector<Primitive*>::const_iterator itr = primitives.begin(); itr != primitives.end(); ++itr) {
                {
                    RenderData dta; 
                    dta.stencilID_ = IDGenerator_->newID();
                    renderInfo_.insert(std::make_pair(*itr, dta));
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
            scissor->enable();
            glClear(GL_STENCIL_BUFFER_BIT);
            depthComplexity = 
                (std::min)(OpenGL::calcMaxDepthComplexity(subtracted, scissor->getCurrentArea()), 
                         subtractedBatches.size());
        }

        channelMgr->request();
        channelMgr->renderToChannel(true);
        
        scissor->enable();
        scissor->store(channelMgr->current());

        glDepthMask(GL_TRUE);
        glStencilMask(OpenGL::stencilMask);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glClearDepth(0.0);      // near clipping plane! essential for algorithm!
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glClearDepth(1.0);

        renderIntersectedFront(intersected);
        switch (algorithm) {
        case NoDepthComplexitySampling:
            subtractPrimitives(subtractedBatches.begin(), subtractedBatches.end(), subtractedBatches.size());
            break;
        case OcclusionQuery:
            subtractPrimitivesWithOcclusionQueries(subtractedBatches.begin(), subtractedBatches.end());
            break;
        case DepthComplexitySampling:
            subtractPrimitives(subtractedBatches.begin(), subtractedBatches.end(), depthComplexity);
            break;
        case DepthComplexityAlgorithmUnused:
            break; // does not happen when invoked correctly           
        }
        renderIntersectedBack(intersected);

        scissor->disable();

        channelMgr->store(channelMgr->current(), primitives, 0);
        channelMgr->free();

        delete scissor;
        delete channelMgr;

        delete IDGenerator_;
    }

} // namespace OpenCSG
