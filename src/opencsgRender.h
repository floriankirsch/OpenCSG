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
// opencsgRender.h 
//
// dispatcher for CSG algorithms implemented in OpenCSG
//

#ifndef __OpenCSG__opencsg_render_h__
#define __OpenCSG__opencsg_render_h__

#include <opencsgConfig.h>
#include <opencsg.h>
#include <vector>

namespace OpenCSG {

    void render(const std::vector<Primitive*>& primitives, 
                Algorithm, 
                DepthComplexityAlgorithm);
        // redeclared from opencsg.h
        // implemented in opencsgRender.cpp

    void renderSCS(const std::vector<Primitive*>& primitives, DepthComplexityAlgorithm);
        // implemented in renderSCS.cpp

    void renderGoldfeather(const std::vector<Primitive*>& primitives);
        // implemented in renderGoldfeather.cpp

    void renderOcclusionQueryGoldfeather(const std::vector<Primitive*>& primitives);
        // implemented in renderGoldfeather.cpp

    void renderDepthComplexitySamplingGoldfeather(const std::vector<Primitive*>& primitives);
        // implemented in renderGoldfeather.cpp

} // namespace OpenCSG

#endif // __OpenCSG__opencsg_render_h__

