// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2002-2010, Florian Kirsch,
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
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

//
// primitiveHelper.cpp
//

#include "opencsgConfig.h"
#include <opencsg.h>
#include <GL/glew.h>
#include "openglHelper.h"
#include "primitiveHelper.h"
#include <algorithm>

namespace OpenCSG {

    namespace Algo {
        bool intersectXY(const Primitive* a, const Primitive* b) {
            float aminx, aminy, aminz, amaxx, amaxy, amaxz,
                  bminx, bminy, bminz, bmaxx, bmaxy, bmaxz;
            a->getBoundingBox(aminx, aminy, aminz, amaxx, amaxy, amaxz);
            b->getBoundingBox(bminx, bminy, bminz, bmaxx, bmaxy, bmaxz);

            return (bmaxx >= aminx) && (amaxx >= bminx)
                && (bmaxy >= aminy) && (amaxy >= bminy);
        }
    
        bool intersectXYZ(const Primitive* a, const Primitive* b) {
            float aminx, aminy, aminz, amaxx, amaxy, amaxz,
                  bminx, bminy, bminz, bmaxx, bmaxy, bmaxz;
            a->getBoundingBox(aminx, aminy, aminz, amaxx, amaxy, amaxz);
            b->getBoundingBox(bminx, bminy, bminz, bmaxx, bmaxy, bmaxz);

            return (bmaxx >= aminx) && (amaxx >= bminx)
                && (bmaxy >= aminy) && (amaxy >= bminy) 
                && (bmaxz >= aminz) && (amaxz >= bminz);
        }

        bool containsXY(const Primitive* a, const Primitive* b) {
            float aminx, aminy, aminz, amaxx, amaxy, amaxz,
                  bminx, bminy, bminz, bmaxx, bmaxy, bmaxz;
            a->getBoundingBox(aminx, aminy, aminz, amaxx, amaxy, amaxz);
            b->getBoundingBox(bminx, bminy, bminz, bmaxx, bmaxy, bmaxz);

            return (aminx >= bminx) && (amaxx <= bmaxx)
                && (aminy >= bminy) && (amaxy <= bmaxy);
        }

        unsigned int getConvexity(const std::vector<Primitive*>& batch) {
            unsigned int convexity=1;
            for (std::vector<Primitive*>::const_iterator itr = batch.begin(); itr != batch.end(); ++itr) {
                unsigned int val = (*itr)->getConvexity();
                if (convexity < val) {
                    convexity = val;
                }
            }

            return convexity;
        }
    } // namespace Algo

    namespace OpenGL {

        unsigned int calcMaxDepthComplexity(const std::vector<Primitive*>& primitives,
                                            const PCArea& area) {

            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

            glDisable(GL_DEPTH_TEST);
            glStencilMask(255);
            glEnable(GL_STENCIL_TEST);
            glStencilFunc(GL_ALWAYS, 0, 255);
            glStencilOp(GL_INCR, GL_INCR, GL_INCR);

            glEnable(GL_CULL_FACE);

            for (std::vector<Primitive*>::const_iterator itr = primitives.begin(); itr != primitives.end(); ++itr) {
                glCullFace((*itr)->getOperation() == Intersection ? GL_BACK : GL_FRONT);
                (*itr)->render();
            }

            int dx = area.maxx - area.minx;
            int dy = area.maxy - area.miny;

            unsigned int size = (8+dx)*dy; // 8 needed due to possible alignment (?)
            static std::vector<GLubyte> buf = std::vector<GLubyte>(size, 0);
            if (buf.size() < size) {
                buf = std::vector<GLubyte>(size, 0);
            }

            glReadPixels(area.minx, area.miny, dx, dy, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &(buf[0]));

            glDisable(GL_CULL_FACE);
            glDisable(GL_STENCIL_TEST);
            glEnable(GL_DEPTH_TEST);

            unsigned char m = *std::max_element(buf.begin(), buf.end());

            unsigned int max = m;

            return max;
        }

        void renderLayer(unsigned int layer, const std::vector<Primitive*>& primitives) {
            glStencilFunc(GL_EQUAL, layer, 255);
            glStencilOp(GL_INCR, GL_INCR, GL_INCR);
            glStencilMask(255);
            glEnable(GL_STENCIL_TEST);
  
            glEnable(GL_CULL_FACE);
            for (std::vector<Primitive*>::const_iterator j = primitives.begin(); j != primitives.end(); ++j) {
                glCullFace((*j)->getOperation() == Intersection ? GL_BACK : GL_FRONT);
                (*j)->render();
            }
            glDisable(GL_CULL_FACE);
        }

    } // namespace OpenGL

} // namespace OpenCSG
