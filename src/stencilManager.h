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
// stencilManager.h 
//
// saves and restores stencil buffer of the main frame buffer if needed
//

#ifndef __OpenCSG__stencil_manager_h__
#define __OpenCSG__stencil_manager_h__

#include <opencsgConfig.h>
#include "area.h"

namespace OpenCSG {

    namespace OpenGL {

        class StencilManager {
        public:
            StencilManager(const PCArea&);
            virtual ~StencilManager();

            const PCArea& getArea() const;

            void clear();
            bool alreadySaved() const;
            virtual void save();
            virtual void restore();

        private:
            const PCArea area_;
            bool saved_;
        };

        StencilManager* getStencilManager(const PCArea&);
            // checks for OpenGL-extensions and returns a stencil manager
            // cannot return zero.
            //
            // when the stencil manager object is not needed anymore,
            // it must be deleted!

    } // namespace OpenGL

} // namespace OpenCSG

#endif // __OpenCSG__stencil_manager_h__
