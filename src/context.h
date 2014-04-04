// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2006-2014, Florian Kirsch
//
// This library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU General Public License, 
// Version 2, as published by the Free Software Foundation.
// As a special exception, you have permission to link this library
// with the CGAL library and distribute executables.
//
// This library is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

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

