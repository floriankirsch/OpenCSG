// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2006-2010, Florian Kirsch
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
// context.cpp
//

#include "opencsgConfig.h"
#include "context.h"
#include "offscreenBuffer.h"

namespace OpenCSG {

    static int gContext = 0;

    /// redeclared from opencsg.h
    void setContext(int context) {
        gContext = context;
    }

    /// redeclared from opencsg.h
    int getContext() {
        return gContext;
    }

    /// redeclared from opencsg.h
    void freeResources() {
        OpenGL::freeResources();
    }

} // namespace OpenCSG
