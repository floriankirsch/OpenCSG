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
// occlusionQueryAdapter.h 
//
// wrappers parts of different occlusion-query extensions conveniently
//

#ifndef __OpenCSG__occlusion_query_adapter_h__
#define __OpenCSG__occlusion_query_adapter_h__

#include <opencsgConfig.h>
#include <GL/glew.h>

namespace OpenCSG {

    namespace OpenGL {

        class OcclusionQueryAdapter {
        public:
            virtual ~OcclusionQueryAdapter() {};

            virtual void beginQuery() = 0;
            virtual void endQuery() = 0;
            virtual unsigned int getQueryResult() = 0;

        protected:
            OcclusionQueryAdapter() {};
        };

        class OcclusionQueryARB : public OcclusionQueryAdapter {
        public:
            OcclusionQueryARB();
            virtual ~OcclusionQueryARB();

            virtual void beginQuery();
            virtual void endQuery();
            virtual unsigned int getQueryResult();

        private:
            GLuint queryObject_;
        };

        class OcclusionQueryNV : public OcclusionQueryAdapter {
        public:
            OcclusionQueryNV();
            virtual ~OcclusionQueryNV();

            virtual void beginQuery();
            virtual void endQuery();
            virtual unsigned int getQueryResult();

        private:
            GLuint queryObject_;
        };

        OcclusionQueryAdapter* getOcclusionQuery();
            // checks for OpenGL-extensions and returns a matching occlusion
            // query object. may return 0 if occlusion queries are not
            // supported by graphics hardware.
            //
            // when the occlusion query object is not needed anymore,
            // it must be deleted!

    } // namespace OpenGL

} // namespace OpenCSG

#endif // __OpenCSG__occlusion_query_adapter_h__


