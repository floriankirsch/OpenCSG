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
// opencsg.h 
//
// main include file for OpenCSG
//

#ifndef __OpenCSG__opencsg_h__
#define __OpenCSG__opencsg_h__

#include "opencsgConfig.h"
#include <vector>

namespace OpenCSG {

    enum Operation { Intersection, Subtraction };

    // abstract base class for CSG primitives. 
    // from this class, derive concrete classes to do CSG rendering
    class Primitive {
    public:
        Primitive(Operation, unsigned int convexity);
        virtual ~Primitive();

        void setOperation(Operation);
        Operation getOperation() const;
            // specifies whether the primitive is intersected or subtracted

        void setConvexity(unsigned int);
        unsigned int getConvexity() const;
            // maximum number of front faces of the primitive at a single
            // position. For example, the convexity of a sphere is 1, and
            // of a torus is 2. Actually the convexity is only used in the 
            // Goldfeather algorithm. For this algorithm, a convexity too 
            // low may result in rendering errors, a convexity too high will
            // reduce rendering performance.

        void setBoundingBox(float  minx, float  miny, float  minz,
                            float  maxx, float  maxy, float  maxz);
        void getBoundingBox(float& minx, float& miny, float& minz,
                            float& maxx, float& maxy, float& maxz) const;
            // Sets the bounding box of the primitive in normalized device 
            // coordinates, i.e. after modelview and projection transfor-
            // mation. Setting this is not mandatory, but allows for various 
            // performance optimizations.

        virtual void render() = 0;
            // abstract render method. overwrite it in derived classes. Be 
            // sure not to alter the modelview or projection (glPush/PopMatrix
            // might be a good idea). Also do not alter colors in your render-
            // method; OpenCSG uses them internally. For best performance, you
            // should only transmit vertex positions; no normals, tex coords 
            // or whatever else.

    private:
        Operation operation_;
        unsigned int convexity_;
        float minx_, miny_, minz_, maxx_, maxy_, maxz_;
    };

    enum Algorithm { Automatic, Goldfeather, SCS };
    enum DepthComplexityAlgorithm { NoDepthComplexitySampling, OcclusionQuery, DepthComplexitySampling };

    void render(const std::vector<Primitive*>& primitives, 
                Algorithm = Automatic, 
                DepthComplexityAlgorithm = NoDepthComplexitySampling);
    // Performs CSG rendering. I.e., this function initializes the z-buffer
    // with the z-values of the CSG product given as array of primitives. 
    // render() does not alter the color buffer, so you have to shade the 
    // primitives using GL_EQUAL depth function afterwards. The content of 
    // the stencil buffer is destroyed when handling concave primitives or 
    // when using the DepthComplexitySampling strategy (see below).
    //
    // render() respects the OpenGL settings of 
    //   - scissor test (CSG calculating will only occur in the specified region)
    //   - stencil test, when only convex primitives are used and no layered 
    //         algorithm is used. Most stenciling ops (increment / decrement 
    //         / zero / one) will not be useful anyway
    //
    // render() ignores
    //   - depth test (always GL_LESS)
    //   - alpha test (used internally)
    //   - cull face  (used internally to distinguish intersected / subtracted 
    //         primitives)
    //
    // The argument Algorithm specifies the method used for CSG rendering:
    //   - Goldfeather: handles convex and concave primitives.
    //   - SCS        : handles only convex primitives
    //   - Automatic  : currently chooses Goldfeather if the primitive vector
    //                  contains concave primitives, else it choses SCS. Also
    //                  sets the DepthComplexityAlgorithm (NoDepthComplexitySampling
    //                  for arrays with few primitives, else OcclusionQuery 
    //                  or at the last resort DepthComplexitySampling)
    //
    // The argument DepthComplexityAlgorithm specifies the strategy for 
    //                  profiting from depth complexity.
    //   - NoDepthComplexitySampling: Does not employ the depth complexity.
    //                  This essentially makes the algorithm O(n²), but with
    //                  low constant costs.
    //   - DepthComplexitySampling: Calculates the depth complexity k using
    //                  the stencil buffer. This makes algorithm O(n*k), but
    //                  with high constant costs. In case of the Goldfeather,
    //                  the literature denotes this as layered Goldfeather 
    //                  algorithm.
    //   - OcclusionQuery: Uses occlusion queries to profit implicitly from
    //                  depth complexity without calculating it. This is 
    //                  especially useful for the SCS algorithm where this 
    //                  strategy is applied at shape level, resulting in a 
    //                  O(n*k') algorithm (where k' <= k), without significant
    //                  constant overhead. This strategy requires hardware 
    //                  occlusion queries (i.E., the OpenGL-extension 
    //                  GL_ARB_occlusion_query or GL_NV_occlusion_query)

} // namespace OpenCSG

#endif // __OpenCSG__opencsg_h__
