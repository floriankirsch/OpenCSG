// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2002-2009, Florian Kirsch,
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
// occlusionQueryAdapter.h 
//
// wrappers parts of different occlusion-query extensions conveniently
//

#ifndef __OpenCSG__occlusion_query_adapter_h__
#define __OpenCSG__occlusion_query_adapter_h__

#include "opencsgConfig.h"

namespace OpenCSG {

    namespace OpenGL {

        class OcclusionQuery {
        public: // abstract base class
            virtual ~OcclusionQuery() {};

            /// begins an occlusion query
            virtual void beginQuery() = 0;
            /// stops an occlusion query
            virtual void endQuery() = 0;
            /// returns the number of fragments that have been put into
            /// the frame buffer between beginQuery() and endQuery().
            virtual unsigned int getQueryResult() = 0;

        protected:
            OcclusionQuery() {};
        };

        /// checks for OpenGL-extensions and returns a matching occlusion
        /// query object. may return 0 if occlusion queries are not
        /// supported by graphics hardware.
        ///
        /// when the occlusion query object is not needed anymore,
        /// it must be deleted!
        OcclusionQuery* getOcclusionQuery();

    } // namespace OpenGL

} // namespace OpenCSG

#endif // __OpenCSG__occlusion_query_adapter_h__


