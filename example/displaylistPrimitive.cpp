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
// displaylistPrimitive.h 
//
// example for a primitive which renders itself using a display list
//

#include <GL/glew.h>
#include "displaylistPrimitive.h"

namespace OpenCSG {

    DisplayListPrimitive::DisplayListPrimitive(unsigned int i, Operation o, unsigned int c)
        : Primitive(o, c)
    {
        displayListId_ = i;
    }

    void DisplayListPrimitive::setDisplayListId(unsigned int i) {
        displayListId_ = i;
    }

    unsigned int DisplayListPrimitive::getDisplayListId() const {
        return displayListId_;
    }

    void DisplayListPrimitive::render() {
        glCallList(displayListId_);
    }

}
