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
// primitiveHelper.h 
//
// trivial Primitive-related stuff
//

#ifndef __OpenCSG__primitive_helper_h__
#define __OpenCSG__primitive_helper_h__

#include <opencsgConfig.h>
#include "area.h"
#include <vector>

namespace OpenCSG {

    class Primitive;

    namespace Algo {

        bool intersectXY(const Primitive* a, const Primitive* b);
            // checks whether primitives intersect in xy direction
        bool intersectXYZ(const Primitive* a, const Primitive* b);
            // checks whether primitives intersect in xyz direction
        unsigned int getConvexity(const std::vector<Primitive*>& primitives);
            // calculates maximum convexity of all primitives in array

    } // namespace Algo

    namespace OpenGL {

        unsigned int calcMaxDepthComplexity(const std::vector<Primitive*>& primitives,
                                            const PCArea& area);
            // calculates depth complexity of the given primitives using the
            // stencil buffer, in the bounding box given by area. 
            // Expects a clean stencil buffer, and does not clear 
            // the stencil buffer afterwards
        void renderLayer(unsigned int layer, const std::vector<Primitive*>& primitives);
            // renders a rendering layer of the given primitives using the
            // stencil buffer. The rendering layers are not ordered from 
            // front to back, so the ordering of the array matters! 
            // Expects a clean stencil buffer, and does not clear the stencil
            // buffer afterwards. 

    } // namespace OpenGL

} // namespace OpenCSG

#endif // __OpenCSG__primitive_helper_h__
