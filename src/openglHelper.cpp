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
// openglHelper.h 
//

#include "opencsgConfig.h"
#include <GL/glew.h>
#include "openglHelper.h"

namespace OpenCSG {

    namespace OpenGL {

        GLfloat projection[16];
        GLfloat modelview[16];
        int canvasPos[4];

        int stencilBits = 0;
        int stencilMax = 0;
        int stencilMask = 0;

        int scissorPos[4];

        void scissor(const PCArea& area) {
            const int dx = area.maxx - area.minx;
            const int dy = area.maxy - area.miny;
            glScissor(area.minx, area.miny, dx, dy);
            glEnable(GL_SCISSOR_TEST);
        }

        void drawQuad() {
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            glBegin(GL_TRIANGLE_STRIP);
                glVertex2f(-1.0, -1.0);
                glVertex2f( 1.0, -1.0);
                glVertex2f(-1.0,  1.0);
                glVertex2f( 1.0,  1.0);
            glEnd();

            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
        }

    } // namespace OpenGL

} // namespace OpenCSG
