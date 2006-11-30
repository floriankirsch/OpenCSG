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
// opencsg.h 
//
// main include file for OpenCSG
//

#ifndef __OpenCSG__opencsg_h__
#define __OpenCSG__opencsg_h__

#include <vector>

namespace OpenCSG {

    enum Operation { Intersection, Subtraction };

    /// Abstract base class for CSG primitives. 
    /// From this class, derive concrete classes to do CSG rendering.
    class Primitive {
    public:
        Primitive(Operation, unsigned int convexity);
        virtual ~Primitive();

        /// Sets the CSG operation.
        /// The operation specifies whether the primitive is intersected
        /// or subtracted.
        void setOperation(Operation);
        /// Returns the CSG operation.
        Operation getOperation() const;

        /// Sets the convexity.
        /// The convexity is the maximum number of front faces of the
        /// primitive at a single position. For example, the convexity
        /// of a sphere is 1, and of a torus is 2. Actually the convexity
        /// is only used in the Goldfeather algorithm. For this algorithm, 
        /// a convexity too low may result in rendering errors, a convexity
        /// too high will reduce rendering performance.
        void setConvexity(unsigned int);
        /// Returns the convexity.
        unsigned int getConvexity() const;

        /// Sets the bounding box.
        /// The bounding box of the primitive is given in normalized device 
        /// coordinates, i.e. after modelview and projection transformation.
        /// Setting it is not required, but allows for various performance
        /// optimizations.
        void setBoundingBox(float  minx, float  miny, float  minz,
                            float  maxx, float  maxy, float  maxz);
        /// Returns the bounding box.
        void getBoundingBox(float& minx, float& miny, float& minz,
                            float& maxx, float& maxy, float& maxz) const;

        /// render() is the abstract render method, to be implemented in
        /// derived classes. In the implementation, be sure not to alter
        /// the modelview or projection (glPushMatrix/glPopMatrix might be
        /// a good idea). Also do not alter colors in your render() method:
        /// OpenCSG uses them internally. For best performance, you should
        /// only transmit vertex positions; no normals, tex coords or
        /// whatever else.
        virtual void render() = 0;

    private:
        Operation mOperation;
        unsigned int mConvexity;
        float mMinx, mMiny, mMinz, mMaxx, mMaxy, mMaxz;
    };

    enum OptionType {
        AlgorithmSetting       = 0,
        DepthComplexitySetting = 1,
        OffscreenSetting       = 2,
        OptionTypeUnused       = 3
    };

    /// Sets an OpenCSG option.
    /// The option parameter specifies which option to set. The newSetting
    /// is the new setting and should be one of the Algorithm, 
    /// DepthComplexityAlgorithm, or OffscreenType enums below.
    void setOption(OptionType option, int newSetting);
    /// Returns the current setting of the provided option type.
    int  getOption(OptionType option);

    /// The Algorithm specifies the method used for CSG rendering:
    ///   - Goldfeather: This algorithm handles convex and concave primitives.
    ///   - SCS        : This algorithm handles only convex primitives
    ///   - Automatic  : This setting currently choses Goldfeather if the
    ///                  primitive vector contains concave primitives, else it
    ///                  choses SCS. Also sets the DepthComplexityAlgorithm
    ///                  (NoDepthComplexitySampling for arrays with few
    ///                  primitives, else OcclusionQuery or at the last resort
    ///                  DepthComplexitySampling). This setting is the default.
    ///   - AlgorithmUnused : For use with setOption, this value is invalid. 
    ///                  As parameter of the render() function, specifies to
    ///                  read all OpenCSG settings from the settings set with
    ///                  setOption() and not from the parameter list of render().
    enum Algorithm {
        Automatic        = 0,
        Goldfeather      = 1,
        SCS              = 2,
        AlgorithmUnused  = 3
    };

    /// The DepthComplexityAlgorithm specifies the strategy for profiting
    /// from depth complexity when performing the CSG rendering.
    ///   - NoDepthComplexitySampling: Does not employ the depth complexity.
    ///                  This essentially makes the algorithm O(n²), but with
    ///                  low constant costs.
    ///   - DepthComplexitySampling: Calculates the depth complexity k using
    ///                  the stencil buffer. This makes algorithm O(n*k), but
    ///                  with high constant costs. In case of the Goldfeather,
    ///                  the literature denotes this as layered Goldfeather 
    ///                  algorithm.
    ///   - OcclusionQuery: Uses occlusion queries to profit implicitly from
    ///                  depth complexity without calculating it. This is 
    ///                  especially useful for the SCS algorithm where this 
    ///                  strategy is applied at shape level, resulting in a 
    ///                  O(n*k') algorithm (where k' <= k), without significant
    ///                  constant overhead. This strategy requires hardware 
    ///                  occlusion queries (i.E., the OpenGL-extension 
    ///                  GL_ARB_occlusion_query or GL_NV_occlusion_query)
    ///   - DepthComplexityAlgorithmUnused: Invalid input. 
    enum DepthComplexityAlgorithm {
        NoDepthComplexitySampling      = 0,
        OcclusionQuery                 = 1,
        DepthComplexitySampling        = 2,
        DepthComplexityAlgorithmUnused = 3
    };

    /// The OffscreenType sets the type of offscreen buffer which is used for
    /// the internal calculations. 
    ///   - AutomaticOffscreenType: Chooses internally depending on available
    ///                  OpenGL extensions. If graphics hardware both support
    ///                  frame buffer objects and PBuffers, PBuffers are chosen.
    ///   - FrameBufferObject: Uses frame buffer objects. This method does 
    ///                  not require context switches on the graphics hardware
    ///                  to change between offscreen and main frame buffer, so
    ///                  in theory this method should be faster.
    ///   - PBuffer: Uses PBuffers. This is the older offscreen type, which
    ///                  is likely to work with older graphics hardware and
    ///                  drivers.
    ///   - OffscreenTypeUnused: Invalid input. 
    enum OffscreenType {
        AutomaticOffscreenType = 0,
        FrameBufferObject      = 1,
        PBuffer                = 2,
        OffscreenTypeUnused    = 3
    };

    /// The function render() performs CSG rendering. The function initializes 
    /// the z-buffer with the z-values of the CSG product given as array of 
    /// primitives. It does not alter the color buffer, so you have to shade
    /// the primitives using GL_EQUAL depth function afterwards. The content
    /// of the stencil buffer is destroyed when handling concave primitives or 
    /// when using the DepthComplexitySampling strategy.
    ///
    /// Algorithm and DepthComplexityAlgorithm parameter can be provided and,
    /// if provided, supercede the options set by setOptioni(). In future
    /// versions of OpenCSG, these parameters will probably be removed.
    ///
    /// render() respects the OpenGL settings of 
    ///   - scissor test (CSG calculating will only occur in the specified region)
    ///   - stencil test, when only convex primitives are used and no layered 
    ///         algorithm is used. Most stenciling ops (increment / decrement 
    ///         / zero / one) will not be useful anyway.
    ///
    /// render() ignores
    ///   - depth test (always GL_LESS)
    ///   - alpha test (used internally)
    ///   - cull face  (used internally to distinguish intersected / subtracted 
    ///         primitives)
    void render(const std::vector<Primitive*>& primitives, 
                Algorithm = AlgorithmUnused, 
                DepthComplexityAlgorithm = NoDepthComplexitySampling);

} // namespace OpenCSG

#endif // __OpenCSG__opencsg_h__
