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
                // class for saving and restoring the stencil buffer in the main
                // frame buffer. The area denotes the region in pixel coordinates
                // which can be saved restored. 

                // Currently this class is deactivated; the stencil buffer will
                // not be saved!
            virtual ~StencilManager();

            const PCArea& getArea() const;

            void clear();
                // clears the stencil buffer, possibly saving a copy of the
                // current stencil buffer in the given region.
            bool alreadySaved() const;
                // checks whether the stencil buffer has been already saved

            virtual void save();
                // saves a copy of the current stencil buffer in the given region
            virtual void restore();
                // restores the current stencil buffer in the given region

        private:
            const PCArea area_;
            bool saved_;
        };

        StencilManager* getStencilManager(const PCArea&);
            // checks for OpenGL-extensions and returns a stencil manager.
            // cannot return zero, but can return a stencil manager which
            // does not saves / restores the stencil buffer at all.

    } // namespace OpenGL

} // namespace OpenCSG

#endif // __OpenCSG__stencil_manager_h__
