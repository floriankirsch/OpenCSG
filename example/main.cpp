// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2002-2010, Florian Kirsch,
// Hasso-Plattner-Institute at the University of Potsdam, Germany
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
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

//
// main.cpp
//
// simple example program for OpenCSG using Glut
//

#include <GL/glew.h>
#include <GL/glut.h>
#include <opencsg.h>
#include "displaylistPrimitive.h"
#include <iostream>
#include <sstream>
#include <string>

enum { 
    CSG_BASIC, CSG_WIDGET, CSG_GRID2D, CSG_GRID3D, CSG_CONCAVE,

    ALGO_AUTOMATIC, GF_STANDARD, GF_DC, GF_OQ, SCS_STANDARD, SCS_DC, SCS_OQ,

    OFFSCREEN_AUTOMATIC, OFFSCREEN_FBO, OFFSCREEN_PBUFFER
};

std::vector<OpenCSG::Primitive*> primitives;

bool               spin = true;
float              rot = 0;
std::ostringstream fpsStream;

void clearPrimitives() {
    for (std::vector<OpenCSG::Primitive*>::const_iterator i = primitives.begin(); i != primitives.end(); ++i) {
        OpenCSG::DisplayListPrimitive* p = 
            static_cast<OpenCSG::DisplayListPrimitive*>(*i);
        glDeleteLists(1, p->getDisplayListId());
        delete p;
    }
    
    primitives.clear();
}

void solidCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks) {

    GLUquadricObj* qobj = gluNewQuadric();

    gluCylinder(qobj, radius, radius, height, slices, stacks);
    glScalef(-1.0, 1.0, -1.0);
    gluDisk(qobj, 0.0, radius, slices, stacks);
    glScalef(-1.0, 1.0, -1.0);
    glTranslatef(0.0, 0.0, height);
    gluDisk(qobj, 0.0, radius, slices, stacks);

    gluDeleteQuadric(qobj);    
}

void setBasicShape() {

    clearPrimitives();

    GLuint id1 = glGenLists(1);
    glNewList(id1, GL_COMPILE);
    glPushMatrix();
    glTranslatef(-0.25, 0.0, 0.0);
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();
    glEndList();

    GLuint id2 = glGenLists(1);
    glNewList(id2, GL_COMPILE);
    glPushMatrix();
    glTranslatef(0.25, 0.0, 0.0);
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();
    glEndList();

    GLuint id3 = glGenLists(1);
    glNewList(id3, GL_COMPILE);
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.5);
    glScalef(0.5, 0.5, 2.0);
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();
    glEndList();

    primitives.push_back(new OpenCSG::DisplayListPrimitive(id1, OpenCSG::Intersection, 1));
    primitives.push_back(new OpenCSG::DisplayListPrimitive(id2, OpenCSG::Intersection, 1));
    primitives.push_back(new OpenCSG::DisplayListPrimitive(id3, OpenCSG::Subtraction, 1));
}

void setWidget() {
    
    clearPrimitives();

    GLuint id1 = glGenLists(1);
    glNewList(id1, GL_COMPILE);
    glutSolidSphere(1.2, 20, 20);
    glEndList();

    GLuint id2 = glGenLists(1);
    glNewList(id2, GL_COMPILE);
    glutSolidCube(1.8);
    glEndList();

    GLuint id3 = glGenLists(1);
    glNewList(id3, GL_COMPILE);
    glPushMatrix();
    glTranslatef(0.0, 0.0, -1.25);
    solidCylinder(0.6, 2.5, 20, 20);
    glPopMatrix();
    glEndList();

    GLuint id4 = glGenLists(1);
    glNewList(id4, GL_COMPILE);
    glPushMatrix();
    glRotatef(90, 0.0, 1.0, 0.0);
    glTranslatef(0.0, 0.0, -1.25);
    solidCylinder(0.6, 2.5, 20, 20);
    glPopMatrix();
    glEndList();

    GLuint id5 = glGenLists(1);
    glNewList(id5, GL_COMPILE);
    glPushMatrix();
    glRotatef(90, 1.0, 0.0, 0.0);
    glTranslatef(0.0, 0.0, -1.25);
    solidCylinder(0.6, 2.5, 20, 20);
    glPopMatrix();
    glEndList();

    primitives.push_back(new OpenCSG::DisplayListPrimitive(id1, OpenCSG::Intersection, 1));
    primitives.push_back(new OpenCSG::DisplayListPrimitive(id2, OpenCSG::Intersection, 1));
    primitives.push_back(new OpenCSG::DisplayListPrimitive(id3, OpenCSG::Subtraction, 1));
    primitives.push_back(new OpenCSG::DisplayListPrimitive(id4, OpenCSG::Subtraction, 1));
    primitives.push_back(new OpenCSG::DisplayListPrimitive(id5, OpenCSG::Subtraction, 1));
}

void setGrid2D() {
    
    clearPrimitives();

    GLuint id1 = glGenLists(1);
    glNewList(id1, GL_COMPILE);
    glPushMatrix();
    glScalef(1.0f, 0.2f, 1.0f);
    glTranslatef(0.0f, -1.25f, 0.0f);
    glutSolidCube(2.5);
    glPopMatrix();
    glEndList();

    primitives.push_back(new OpenCSG::DisplayListPrimitive(id1, OpenCSG::Intersection, 1));

    for (int x=-2; x<=2; ++x) {
        for (int z=-2; z<=2; ++z) {
            GLuint id = glGenLists(1);
            glNewList(id, GL_COMPILE);
            glPushMatrix();
            glTranslatef(x*0.5, 0.0, z*0.5);
            glutSolidSphere(0.22, 15, 15);
            glPopMatrix();
            glEndList();

            primitives.push_back(new OpenCSG::DisplayListPrimitive(id, OpenCSG::Subtraction, 1));
        }
    }
}

void setGrid3D() {
    
    clearPrimitives();

    GLuint id1 = glGenLists(1);
    glNewList(id1, GL_COMPILE);
    glutSolidCube(2.0);
    glEndList();

    primitives.push_back(new OpenCSG::DisplayListPrimitive(id1, OpenCSG::Intersection, 1));

    for (int x=-1; x<=1; ++x) {
        for (int y=-1; y<=1; ++y) {
            for (int z=-1; z<=1; ++z) {
                GLuint id = glGenLists(1);
                glNewList(id, GL_COMPILE);
                glPushMatrix();
                glTranslatef(x, y, z);
                glutSolidSphere(0.58, 20, 20);
                glPopMatrix();
                glEndList();

                primitives.push_back(new OpenCSG::DisplayListPrimitive(id, OpenCSG::Subtraction, 1));
            }
        }
    }
}

void setConcave() {
    
    clearPrimitives();

    GLuint id1 = glGenLists(1);
    glNewList(id1, GL_COMPILE);
    glutSolidTorus(0.6, 1.0, 25, 25);
    glEndList();
    primitives.push_back(new OpenCSG::DisplayListPrimitive(id1, OpenCSG::Intersection, 2));

    for (unsigned int i=0; i<4; ++i) {
        GLuint id = glGenLists(1);
        glNewList(id, GL_COMPILE);
        glPushMatrix();
        glRotatef(90, 0.0, 1.0, 0.0);
        glRotatef(i*90 + 45, 1.0, 0.0, 0.0);
        glTranslatef(0.0, 1.0, 0.0);
        glutSolidTorus(0.3, 0.6, 15, 15);
        glPopMatrix();
        glEndList();
        primitives.push_back(new OpenCSG::DisplayListPrimitive(id, OpenCSG::Subtraction, 2));
    }

    GLuint id3 = glGenLists(1);
    glNewList(id3, GL_COMPILE);
    glPushMatrix();
    glRotatef(90, 0.0, 1.0, 0.0);
    glTranslatef(0.0f, 0.0f, -1.65f);
    solidCylinder(0.3, 3.3, 20, 20);
    glPopMatrix();
    glEndList();
    primitives.push_back(new OpenCSG::DisplayListPrimitive(id3, OpenCSG::Subtraction, 1));

    GLuint id4 = glGenLists(1);
    glNewList(id4, GL_COMPILE);
    glPushMatrix();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, -1.65f);
    solidCylinder(0.3, 3.3, 20, 20);
    glPopMatrix();
    glEndList();
    primitives.push_back(new OpenCSG::DisplayListPrimitive(id4, OpenCSG::Subtraction, 1));
}

void renderfps() {
    glDisable(GL_DEPTH_TEST);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(-1.0, -1.0);
    glDisable(GL_LIGHTING);
    std::string s = fpsStream.str();
    for (unsigned int i=0; i<s.size(); ++i) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, s[i]);
    }
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}

void display()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 2.0, 5.0,  /* eye is at (0,2,5) */
              0.0, 0.0, 0.0,  /* center is at (0,0,0) */
              0.0, 1.0, 0.0); /* up is in positive Y direction */
    glRotatef(rot, 0.0, 1.0, 0.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    OpenCSG::render(primitives);
    glDepthFunc(GL_EQUAL);
    for (std::vector<OpenCSG::Primitive*>::const_iterator i = primitives.begin(); i != primitives.end(); ++i) {
        (*i)->render();
    }
    glDepthFunc(GL_LESS);

    renderfps();

    glutSwapBuffers();
}

void idle() {

    static int ancient = 0;
    static int last = 0;
    static int msec = 0;        
    last = msec;
    msec = glutGet(GLUT_ELAPSED_TIME);
    if (spin) {
        rot += (msec-last)/10.0; 
        while (rot >= 360) rot -= 360;
    }

    static int fps = 0;
    if (last / 1000 != msec / 1000) {
        
        float correctedFps = static_cast<float>(fps) * 1000.0 / static_cast<float>(msec - ancient);
        fpsStream.str("");
        fpsStream << "fps: " << correctedFps << std::ends;

        ancient = msec;
        fps = 0;
    }

    display();

    ++fps;
}

void key(unsigned char k, int, int) {
    switch (k) {
    case ' ': 
        spin = !spin; 
        break;    
    default:
        break;
    }
    display();
}

void menu(int value) {
    switch (value) {
    case CSG_BASIC:      setBasicShape();    break;
    case CSG_WIDGET:     setWidget();        break;
    case CSG_GRID2D:     setGrid2D();        break;
    case CSG_GRID3D:     setGrid3D();        break;
    case CSG_CONCAVE:    setConcave();       break;

    case ALGO_AUTOMATIC: OpenCSG::setOption(OpenCSG::AlgorithmSetting, OpenCSG::Automatic);
                         break;
    case GF_STANDARD:    OpenCSG::setOption(OpenCSG::AlgorithmSetting, OpenCSG::Goldfeather);
                         OpenCSG::setOption(OpenCSG::DepthComplexitySetting, OpenCSG::NoDepthComplexitySampling);
                         break;
    case GF_DC:          OpenCSG::setOption(OpenCSG::AlgorithmSetting, OpenCSG::Goldfeather);
                         OpenCSG::setOption(OpenCSG::DepthComplexitySetting, OpenCSG::DepthComplexitySampling);
                         break;
    case GF_OQ:          OpenCSG::setOption(OpenCSG::AlgorithmSetting, OpenCSG::Goldfeather);
                         OpenCSG::setOption(OpenCSG::DepthComplexitySetting, OpenCSG::OcclusionQuery);
                         break;
    case SCS_STANDARD:   OpenCSG::setOption(OpenCSG::AlgorithmSetting, OpenCSG::SCS);
                         OpenCSG::setOption(OpenCSG::DepthComplexitySetting, OpenCSG::NoDepthComplexitySampling);
                         break;
    case SCS_DC:         OpenCSG::setOption(OpenCSG::AlgorithmSetting, OpenCSG::SCS);
                         OpenCSG::setOption(OpenCSG::DepthComplexitySetting, OpenCSG::DepthComplexitySampling);
                         break;
    case SCS_OQ:         OpenCSG::setOption(OpenCSG::AlgorithmSetting, OpenCSG::SCS);
                         OpenCSG::setOption(OpenCSG::DepthComplexitySetting, OpenCSG::OcclusionQuery);
                         break;

    case OFFSCREEN_AUTOMATIC: OpenCSG::setOption(OpenCSG::OffscreenSetting, OpenCSG::AutomaticOffscreenType);
                              break;
    case OFFSCREEN_FBO:       OpenCSG::setOption(OpenCSG::OffscreenSetting, OpenCSG::FrameBufferObject);
                              break;
    case OFFSCREEN_PBUFFER:   OpenCSG::setOption(OpenCSG::OffscreenSetting, OpenCSG::PBuffer);
                              break;

    default: break;
    }
    display();
}

void init()
{
    // gray background
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

    // Enable two OpenGL lights
    GLfloat light_diffuse[] = {1.0, 0.0, 0.0, 1.0};       // Red diffuse light
    GLfloat light_position0[] = {-1.0, -1.0, -1.0, 0.0};  // Infinite light location
    GLfloat light_position1[] = {1.0, 1.0, 1.0, 0.0};     // Infinite light location

    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glEnable(GL_LIGHT0);  
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

    // Use depth buffering for hidden surface elimination
    glEnable(GL_DEPTH_TEST);

    // Setup the view of the CSG shape
    glMatrixMode(GL_PROJECTION);
    gluPerspective(40.0, 1.0, 1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);

    // starting CSG shape
    setWidget();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(512, 512);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
    glutCreateWindow("OpenCSG example application");

    int err = glewInit();
    if (GLEW_OK != err) {
        // problem: glewInit failed, something is seriously wrong
        std::cerr << "GLEW Error: " << glewGetErrorString(err) << std::endl;
        return 1;
    }  

    int menuShape     = glutCreateMenu(menu);
    glutAddMenuEntry("Simple",  CSG_BASIC);
    glutAddMenuEntry("Widget",  CSG_WIDGET);
    glutAddMenuEntry("2D-Grid", CSG_GRID2D);
    glutAddMenuEntry("3D-Grid", CSG_GRID3D);
    glutAddMenuEntry("Concave", CSG_CONCAVE);
    
    int menuAlgorithm = glutCreateMenu(menu);
    glutAddMenuEntry("Automatic", ALGO_AUTOMATIC);
    glutAddMenuEntry("Goldfeather standard",GF_STANDARD);
    glutAddMenuEntry("Goldfeather depth complexity sampling", GF_DC);
    glutAddMenuEntry("Goldfeather occlusion query", GF_OQ);
    glutAddMenuEntry("SCS standard", SCS_STANDARD);
    glutAddMenuEntry("SCS depth complexity sampling", SCS_DC);
    glutAddMenuEntry("SCS occlusion query", SCS_OQ);

    int menuSettings = glutCreateMenu(menu);
    glutAddMenuEntry("Automatic", OFFSCREEN_AUTOMATIC);
    glutAddMenuEntry("Frame buffer object", OFFSCREEN_FBO);
    glutAddMenuEntry("PBuffer", OFFSCREEN_PBUFFER);

    glutCreateMenu(menu);
    glutAddSubMenu("CSG Shapes", menuShape);
    glutAddSubMenu("CSG Algorithms", menuAlgorithm);
    glutAddSubMenu("Settings", menuSettings);

    // connect to right mouse button
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutDisplayFunc(display);
    glutKeyboardFunc(key);

    menu(OFFSCREEN_AUTOMATIC);

    glutIdleFunc(idle);
    init();
    glutMainLoop();

    return 0;
}

