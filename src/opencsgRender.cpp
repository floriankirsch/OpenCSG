// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2002, 2003
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
// opencsgRender.cpp
//

#include <opencsgConfig.h>
#include <opencsg.h>
#include <GL/glew.h>
#include "opencsgRender.h"
#include "primitiveHelper.h"

namespace OpenCSG {

    namespace {

        Algorithm chooseAlgorithm(const std::vector<Primitive*>& primitives) {
            if (Algo::getConvexity(primitives) >= 2) {
                return Goldfeather;                
            }
            return SCS;
        }

        DepthComplexityAlgorithm chooseDepthComplexityAlgorithm(const std::vector<Primitive*>& primitives) {
            if (primitives.size() > 20) {
                if (GLEW_NV_occlusion_query && primitives.size() > 40) {
                    return DepthComplexitySampling;
                }
                return OcclusionQuery;
            }
            return NoDepthComplexitySampling;
        }

    } // unnamed namespace

    void render(const std::vector<Primitive*>& primitives, 
                Algorithm algorithm, 
                DepthComplexityAlgorithm depthComplexityAlgorithm) {

        if (algorithm == Automatic) {
            algorithm = chooseAlgorithm(primitives);
            depthComplexityAlgorithm = chooseDepthComplexityAlgorithm(primitives);
        }

        if (depthComplexityAlgorithm == OcclusionQuery && !GLEW_NV_occlusion_query) {
            // hardware support is missing. issue a warning?
            depthComplexityAlgorithm = DepthComplexitySampling;
        }

        switch (algorithm) {
        case Goldfeather:
            switch (depthComplexityAlgorithm) {
            case NoDepthComplexitySampling: 
                renderGoldfeather(primitives);
                break;
            case OcclusionQuery:
                renderOcclusionQueryGoldfeather(primitives);
                break;
            case DepthComplexitySampling:
                renderDepthComplexitySamplingGoldfeather(primitives);
                break;
            }
            break;

        case SCS:
            renderSCS(primitives, depthComplexityAlgorithm);
            break;
        
        default:
            ;
        }
    }

} // namespace OpenCSG
