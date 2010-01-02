// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2002-2010, Florian Kirsch,
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
// occlusionQueryAdapter.cpp
//

#include "opencsgConfig.h"
#include <GL/glew.h>
#include "occlusionQuery.h"

namespace OpenCSG {

    namespace OpenGL {

        class OcclusionQueryARB : public OcclusionQuery {
        public:
            OcclusionQueryARB();
            virtual ~OcclusionQueryARB();

            virtual void beginQuery();
            virtual void endQuery();
            virtual unsigned int getQueryResult();

        private:
            GLuint mQueryObject;
        };

        OcclusionQueryARB::OcclusionQueryARB() {
            glGenQueriesARB(1, &mQueryObject);
        }

        OcclusionQueryARB::~OcclusionQueryARB() {
            glDeleteQueriesARB(1, &mQueryObject);
        }

        void OcclusionQueryARB::beginQuery() {
            glBeginQueryARB(GL_SAMPLES_PASSED_ARB, mQueryObject);
        }

        void OcclusionQueryARB::endQuery() {
            glEndQueryARB(GL_SAMPLES_PASSED_ARB);
        }

        unsigned int OcclusionQueryARB::getQueryResult() {
            unsigned int fragmentCount;
            glGetQueryObjectuivARB(mQueryObject, GL_QUERY_RESULT_ARB, &fragmentCount);
            return fragmentCount;
        }



        class OcclusionQueryNV : public OcclusionQuery {
        public:
            OcclusionQueryNV();
            virtual ~OcclusionQueryNV();

            virtual void beginQuery();
            virtual void endQuery();
            virtual unsigned int getQueryResult();

        private:
            GLuint mQueryObject;
        };

        OcclusionQueryNV::OcclusionQueryNV() {
            glGenOcclusionQueriesNV(1, &mQueryObject);
        }

        OcclusionQueryNV::~OcclusionQueryNV() {
            glDeleteOcclusionQueriesNV(1, &mQueryObject);
        }

        void OcclusionQueryNV::beginQuery() {
            glBeginOcclusionQueryNV(mQueryObject);
        }

        void OcclusionQueryNV::endQuery() {
            glEndOcclusionQueryNV();
        }

        unsigned int OcclusionQueryNV::getQueryResult() {
            unsigned int fragmentCount;
            glGetOcclusionQueryuivNV(mQueryObject, GL_PIXEL_COUNT_NV, &fragmentCount);
            return fragmentCount;
        }



        OcclusionQuery* getOcclusionQuery() {
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
