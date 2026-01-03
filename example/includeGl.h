// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2023-2026, Florian Kirsch
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110 - 1301 USA.

//
// includeGl.h
//
// Includes glut.h. Note that this implicitely also includes GL/gl.h
// This is because the example program no longer depend on GLEW
// anymore and does not include glew.h. But including GL/gl.h
// directly tends to be non-portable.
//

#ifndef __OpenCSG__include_gl_h__
#define __OpenCSG__include_gl_h__

#ifdef __APPLE__
#include <GLUT/glut.h>
#elif  _WIN32
// glut include under windows, assuming that freeglut binary has been
// directly put into the opencsg main directory (e.g., try to use the
// freeglut 3.0.0 MSVC Package that is available from
// https://www.transmissionzero.co.uk/software/freeglut-devel)
#include <../freeglut/include/GL/glut.h>
#else
#include <GL/glut.h>
#endif

#endif
