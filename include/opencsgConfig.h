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
// opencsgConfig.h 
//
// compiler specific settings for OpenCSG
//

#ifndef __OpenCSG__opencsg_config_h__
#define __OpenCSG__opencsg_config_h__

// WINDOWS
#ifdef WIN32

// blend out some compiler warnings
#ifdef __INTEL_COMPILER // intel compiler

# pragma warning(disable: 985) // identifier ... was truncated in debug information

#elif _MSC_VER // microsoft visual studio compiler without intel compiler

# pragma warning(disable: 4786) // identifier was truncated to X characters in the debug information

#endif // __INTEL_COMPILER || _MSC_VER

#ifdef _MSC_VER // microsoft visual studio compiler and intel compiler
// MSVC does not have std::min and std::max unless using .NET (version 1300, VC++ 6 was version 1200)
// For .net, this still appears to be the case if including some windows headers.
// instead it defines _cpp_max and _cpp_min

    #ifdef min
    #undef min
    #endif

    #ifdef max
    #undef max
    #endif

    #define max _cpp_max
    #define min _cpp_min

#endif // _MSC_VER

#endif // WIN32

#endif // __OpenCSG__opencsg_config_h__
