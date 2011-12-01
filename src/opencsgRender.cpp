// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2002-2011, Florian Kirsch,
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
// opencsgRender.cpp
//

#include "opencsgConfig.h"
#include <opencsg.h>
#include <GL/glew.h>
#include "opencsgRender.h"
#include "primitiveHelper.h"
#include "settings.h"

namespace OpenCSG {

    namespace {

        bool haveHardwareOcclusionQueries() {
            return GLEW_ARB_occlusion_query || GLEW_NV_occlusion_query;
        }

        Algorithm chooseAlgorithm(const std::vector<Primitive*>& primitives) {
            if (Algo::getConvexity(primitives) >= 2) {
                return Goldfeather;
            }
            return SCS;
        }

        DepthComplexityAlgorithm chooseDepthComplexityAlgorithm(const std::vector<Primitive*>& primitives) {
            if (!haveHardwareOcclusionQueries() && primitives.size() > 40) {
                return DepthComplexitySampling;
            }

            if (haveHardwareOcclusionQueries() && primitives.size() > 20) {
                return OcclusionQuery;
            }

            return NoDepthComplexitySampling;
        }

    } // unnamed namespace


    void renderDispatch(const std::vector<Primitive*>& primitives, 
                        Algorithm algorithm, 
                        DepthComplexityAlgorithm depthComplexityAlgorithm)
    {
        if (primitives.empty()) {
            return;
        }

        if (algorithm == Automatic) {
            algorithm = chooseAlgorithm(primitives);
            depthComplexityAlgorithm = chooseDepthComplexityAlgorithm(primitives);
        }

        if (depthComplexityAlgorithm == OcclusionQuery && !haveHardwareOcclusionQueries()) {
            // hardware support is missing. issue a warning?
            depthComplexityAlgorithm = DepthComplexitySampling;
        }

        if (   algorithm != Automatic
            && depthComplexityAlgorithm != DepthComplexityAlgorithmUnused
        ) {
            switch (algorithm) {
            case Goldfeather:
                renderGoldfeather(primitives, depthComplexityAlgorithm);
                break;
    
            case SCS:
                renderSCS(primitives, depthComplexityAlgorithm);
                break;
            
            default:
                break;
            }
        }
    }

    void render(const std::vector<Primitive*>& primitives)
    {
        Algorithm algorithm = (Algorithm)getOption(AlgorithmSetting);
        DepthComplexityAlgorithm depthComplexityAlgorithm = (DepthComplexityAlgorithm)getOption(DepthComplexitySetting);

        renderDispatch(primitives, algorithm, depthComplexityAlgorithm);
    }

    void render(const std::vector<Primitive*>& primitives, 
                Algorithm algorithm, 
                DepthComplexityAlgorithm depthComplexityAlgorithm)
    {
        if (algorithm == AlgorithmUnused) {
            render(primitives);
        }
        else {
            renderDispatch(primitives, algorithm, depthComplexityAlgorithm);
        }
    }

} // namespace OpenCSG
