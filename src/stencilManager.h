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
// stencilManager.h 
//
// saves and restores stencil buffer of the main frame buffer if needed
//

#ifndef __OpenCSG__stencil_manager_h__
#define __OpenCSG__stencil_manager_h__

#include "opencsgConfig.h"
#include "area.h"

namespace OpenCSG {

    namespace OpenGL {

        class StencilManager {
        public:
            /// class for saving and restoring the stencil buffer in the main
            /// frame buffer. The area denotes the region in pixel coordinates
            /// which can be saved restored. 
            /// Currently this class is deactivated; the stencil buffer will
            /// not be saved!
            StencilManager(const PCArea&);
            virtual ~StencilManager();

            /// Returns the area of the stencil buffer that is saved / restored.
            const PCArea& getArea() const;
            
            /// clears the stencil buffer, possibly saving a copy of the
            /// current stencil buffer in the given region.
            void clear();
            /// checks whether the stencil buffer has been already saved
            bool alreadySaved() const;

            /// saves a copy of the current stencil buffer in the given region
            virtual void save();
            /// restores the current stencil buffer in the given region
            virtual void restore();
                

        private:
            const PCArea mArea;
            bool mSaved;
        };

        // checks for OpenGL-extensions and returns a stencil manager.
        // cannot return zero, but can return a stencil manager which
        // does not saves / restores the stencil buffer at all.
        StencilManager* getStencilManager(const PCArea&);

    } // namespace OpenGL

} // namespace OpenCSG

#endif // __OpenCSG__stencil_manager_h__
