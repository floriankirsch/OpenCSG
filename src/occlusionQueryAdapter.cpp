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
// occlusionQueryAdapter.cpp
//

#include <opencsgConfig.h>
#include <GL/glew.h>
#include "occlusionQueryAdapter.h"

namespace OpenCSG {

    namespace OpenGL {

        OcclusionQueryARB::OcclusionQueryARB() {
            glGenQueriesARB(1, &queryObject_);
        }

        OcclusionQueryARB::~OcclusionQueryARB() {
            glDeleteQueriesARB(1, &queryObject_);
        }

        void OcclusionQueryARB::beginQuery() {
            glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queryObject_);
        }

        void OcclusionQueryARB::endQuery() {
            glEndQueryARB(GL_SAMPLES_PASSED_ARB);
        }

        unsigned int OcclusionQueryARB::getQueryResult() {
            unsigned int fragmentCount;
            glGetQueryObjectuivARB(queryObject_, GL_QUERY_RESULT_ARB, &fragmentCount);
            return fragmentCount;
        }



        OcclusionQueryNV::OcclusionQueryNV() {
            glGenOcclusionQueriesNV(1, &queryObject_);
        }

        OcclusionQueryNV::~OcclusionQueryNV() {
            glDeleteOcclusionQueriesNV(1, &queryObject_);
        }

        void OcclusionQueryNV::beginQuery() {
            glBeginOcclusionQueryNV(queryObject_);
        }

        void OcclusionQueryNV::endQuery() {
            glEndOcclusionQueryNV();
        }

        unsigned int OcclusionQueryNV::getQueryResult() {
            unsigned int fragmentCount;
            glGetOcclusionQueryuivNV(queryObject_, GL_PIXEL_COUNT_NV, &fragmentCount);
            return fragmentCount;
        }



        OcclusionQueryAdapter* getOcclusionQuery() {
            if (GLEW_ARB_occlusion_query) {
                return new OcclusionQueryARB;
            }

            if (GLEW_NV_occlusion_query) {
                return new OcclusionQueryNV;
            }

            return 0;
        }

    } // namespace OpenGL

} // namespace OpenCSG
