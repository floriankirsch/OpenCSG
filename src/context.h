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
// context.h 
//
// OpenCSG context management
//

#ifndef __OpenCSG__context_h__
#define __OpenCSG__context_h__

#include "opencsgConfig.h"
#include <opencsg.h>

namespace OpenCSG {

    /// redeclared from opencsg.h
    void setContext(int context);
    /// redeclared from opencsg.h
    int getContext();
    /// redeclared from opencsg.h
    void freeResources();

} // namespace OpenCSG

#endif // __OpenCSG__context_h__

