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
// displaylistPrimitive.h 
//
// example for a primitive which renders itself using a display list
//

#include <opencsg.h>

namespace OpenCSG {

    class DisplayListPrimitive : public Primitive {
    public:
        /// An object of this class contains the OpenGL id of a display
        /// list that is compiled by the application. render() just invokes
        /// this display list. 
        /// Operation and convexity are just forwarded to the base Primitive class.
        DisplayListPrimitive(unsigned int displayListId_, Operation, unsigned int convexity);

        /// Sets the display list id
        void setDisplayListId(unsigned int);
        /// Returns the display list id
        unsigned int getDisplayListId() const;

        /// Calls the display list.
        virtual void render();

    private:
        unsigned int mDisplayListId;
    };

} // namespace OpenCSG

