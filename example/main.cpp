// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2002-2025, Florian Kirsch,
// Hasso-Plattner-Institute at the University of Potsdam, Germany
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
// main.cpp
//
// simple example program for OpenCSG using Glut
//

#include <opencsg.h>
#include "displaylistPrimitive.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

// include glut.h after stdlib.h to avoid conflict in declaration
// of exit() with Visual Studio 2010
#include "includeGl.h"

enum {
    BENCH_START,

    CSG_BASIC, CSG_WIDGET, CSG_GRID2D, CSG_GRID3D, CSG_CUBERACK, CSG_PIPE, CSG_CONCAVE,

    ALGO_AUTOMATIC, GF_STANDARD, GF_DC, GF_OQ, SCS_STANDARD, SCS_DC, SCS_OQ,

    CAM_OUTSIDE_DEFAULT, CAM_OUTSIDE_PERF, CAM_OUTSIDE_NOPERF, CAM_INSIDE, CAM_INSIDE_DEFAULT,

    GLSL_FIXED_FUNCTION, GLSL_FTRANSFORM, GLSL_MVP_COMBINED, GLSL_MVP_SEPARATE
};

std::vector<OpenCSG::Primitive*> primitives;
std::vector<OpenCSG::Primitive*> primitives2;
std::vector<OpenCSG::Primitive*> primitives3;


bool               spin = true;
bool               inside = false;
float              rot = 0.0f;
std::ostringstream fpsStream;

std::vector<GLuint> displaylistGarbagePile;

bool               benchmode = false;
bool               benchSettingFrameOne = false;
int                benchShape = BENCH_START;
int                benchAlgorithm = GF_STANDARD;
int                benchPerfOption = CAM_OUTSIDE_DEFAULT;

void clearPrimitives(std::vector<OpenCSG::Primitive*> & p)
{
    for (std::vector<OpenCSG::Primitive*>::const_iterator it = p.begin(); it != p.end(); ++it)
    {
        OpenCSG::DisplayListPrimitive* primitive = static_cast<OpenCSG::DisplayListPrimitive*>(*it);
        GLuint id = primitive->getDisplayListId();
        if (std::find(displaylistGarbagePile.begin(), displaylistGarbagePile.end(), id) == displaylistGarbagePile.end())
        {
            glDeleteLists(1, id);
            displaylistGarbagePile.push_back(id);
        }
        delete primitive;
    }

    p.clear();
}

void clearPrimitives()
{
    displaylistGarbagePile.clear();
    clearPrimitives(primitives);
    clearPrimitives(primitives2);
    clearPrimitives(primitives3);
    displaylistGarbagePile.clear();
}

void solidCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks) {

    GLUquadricObj* qobj = gluNewQuadric();

    gluCylinder(qobj, radius, radius, height, slices, stacks);
    glScalef(-1.0f, 1.0f, -1.0f);
    gluDisk(qobj, 0.0, radius, slices, stacks);
    glScalef(-1.0f, 1.0f, -1.0f);
    glTranslatef(0.0f, 0.0f, static_cast<GLfloat>(height));
    gluDisk(qobj, 0.0, radius, slices, stacks);

    gluDeleteQuadric(qobj);    
}

void setBasicShape() {

    clearPrimitives();

    GLuint id1 = glGenLists(1);
    glNewList(id1, GL_COMPILE);
    glPushMatrix();
    glTranslatef(-0.25f, 0.0f, 0.0f);
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();
    glEndList();

    GLuint id2 = glGenLists(1);
    glNewList(id2, GL_COMPILE);
    glPushMatrix();
    glTranslatef(0.25f, 0.0f, 0.0f);
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();
    glEndList();

    GLuint id3 = glGenLists(1);
    glNewList(id3, GL_COMPILE);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.5f);
    glScalef(0.5f, 0.5f, 2.0f);
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
    glTranslatef(0.0f, 0.0f, -1.25f);
    solidCylinder(0.6, 2.5, 20, 20);
    glPopMatrix();
    glEndList();

    GLuint id4 = glGenLists(1);
    glNewList(id4, GL_COMPILE);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, -1.25f);
    solidCylinder(0.6, 2.5, 20, 20);
    glPopMatrix();
    glEndList();

    GLuint id5 = glGenLists(1);
    glNewList(id5, GL_COMPILE);
    glPushMatrix();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, -1.25f);
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
            glTranslatef(x*0.5f, 0.0f, z*0.5f);
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
                glTranslatef(static_cast<GLfloat>(x), static_cast<GLfloat>(y), static_cast<GLfloat>(z));
                glutSolidSphere(0.58, 20, 20);
                glPopMatrix();
                glEndList();

                primitives.push_back(new OpenCSG::DisplayListPrimitive(id, OpenCSG::Subtraction, 1));
            }
        }
    }
}

void setCubeRack() {

    clearPrimitives();

    GLuint id1 = glGenLists(1);
    glNewList(id1, GL_COMPILE);
    glutSolidCube(2.0);
    glEndList();

    primitives.push_back(new OpenCSG::DisplayListPrimitive(id1, OpenCSG::Intersection, 1));

    // mx*x / my*y / mz*z loop all numbers in [-3, 3] in the following order:
    // 3, -3, 2, -2, 1, -1, 0. Compared to the trivial ordering, this makes
    // the CSG rendering less depending on the camera orientation.
    for (int x=3; x>=0; --x) {
        for (int y=3; y>=0; --y) {
            for (int z=3; z>=0; --z) {
                for (int mx=-1; mx<=1 && mx<=x; mx+=2) {
                    for (int my=-1; my<=1 && my<=y; my+=2) {
                        for (int mz=-1; mz<=1 && mz<=z; mz+=2) {
                            GLuint id = glGenLists(1);
                            glNewList(id, GL_COMPILE);
                            glPushMatrix();
                            glTranslatef(float(x*mx)/6.0f, float(y*my)/6.0f, float(z*mz)/6.0f);
                            glutSolidSphere(0.58, 20, 20);
                            glPopMatrix();
                            glEndList();

                            primitives.push_back(new OpenCSG::DisplayListPrimitive(id, OpenCSG::Subtraction, 1));
                        }
                    }
                }
            }
        }
    }
}

void setPipe() {

    clearPrimitives();

    GLuint id1 = glGenLists(1);
    glNewList(id1, GL_COMPILE);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -2.5f);
    solidCylinder(0.6, 2.5, 20, 20);
    glPopMatrix();
    glEndList();

    GLuint id2 = glGenLists(1);
    glNewList(id2, GL_COMPILE);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -2.51f);
    solidCylinder(0.5, 2.52, 20, 20);
    glPopMatrix();
    glEndList();

    GLuint id3 = glGenLists(1);
    glNewList(id3, GL_COMPILE);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, -2.5f);
    solidCylinder(0.6, 2.5, 20, 20);
    glPopMatrix();
    glEndList();

    GLuint id4 = glGenLists(1);
    glNewList(id4, GL_COMPILE);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, -2.51f);
    solidCylinder(0.5, 2.52, 20, 20);
    glPopMatrix();
    glEndList();

    GLuint id5 = glGenLists(1);
    glNewList(id5, GL_COMPILE);
    glutSolidSphere(0.6, 20, 20);
    glEndList();

    GLuint id6 = glGenLists(1);
    glNewList(id6, GL_COMPILE);
    glutSolidSphere(0.5, 20, 20);
    glEndList();

    GLuint id7 = glGenLists(1);
    glNewList(id7, GL_COMPILE);
    glPushMatrix();
    glTranslatef(-0.8f, 1.0f, -0.8f);
    glutSolidCube(2.0);
    glPopMatrix();
    glEndList();

    primitives.push_back(new OpenCSG::DisplayListPrimitive(id1, OpenCSG::Intersection, 1));
    primitives.push_back(new OpenCSG::DisplayListPrimitive(id2, OpenCSG::Subtraction, 1));
    primitives.push_back(new OpenCSG::DisplayListPrimitive(id4, OpenCSG::Subtraction, 1));
    primitives.push_back(new OpenCSG::DisplayListPrimitive(id7, OpenCSG::Subtraction, 1));

    primitives2.push_back(new OpenCSG::DisplayListPrimitive(id3, OpenCSG::Intersection, 1));
    primitives2.push_back(new OpenCSG::DisplayListPrimitive(id2, OpenCSG::Subtraction, 1));
    primitives2.push_back(new OpenCSG::DisplayListPrimitive(id4, OpenCSG::Subtraction, 1));
    primitives2.push_back(new OpenCSG::DisplayListPrimitive(id7, OpenCSG::Subtraction, 1));

    primitives3.push_back(new OpenCSG::DisplayListPrimitive(id5, OpenCSG::Intersection, 1));
    primitives3.push_back(new OpenCSG::DisplayListPrimitive(id6, OpenCSG::Subtraction, 1));
    primitives3.push_back(new OpenCSG::DisplayListPrimitive(id2, OpenCSG::Subtraction, 1));
    primitives3.push_back(new OpenCSG::DisplayListPrimitive(id4, OpenCSG::Subtraction, 1));
    primitives3.push_back(new OpenCSG::DisplayListPrimitive(id7, OpenCSG::Subtraction, 1));
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
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(i*90.0f + 45.0f, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 1.0f, 0.0f);
        glutSolidTorus(0.3, 0.6, 15, 15);
        glPopMatrix();
        glEndList();
        primitives.push_back(new OpenCSG::DisplayListPrimitive(id, OpenCSG::Subtraction, 2));
    }

    GLuint id3 = glGenLists(1);
    glNewList(id3, GL_COMPILE);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
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
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2f(-1.0f, -1.0f);
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

    if (inside)
    {
       gluLookAt(0.0, 0.25, 0.0,  /* eye is slightly above (0,0,0) */
                 1.0,-0.25, 0.0,  /* looking to the right and a bit down */
                 0.0, 1.0, 0.0);  /* up is in positive Y direction */
    }
    else
    {
        gluLookAt(0.0, 2.0, 5.0,  /* eye is at (0,2,5) */
                  0.0, 0.0, 0.0,  /* center is at (0,0,0) */
                  0.0, 1.0, 0.0); /* up is in positive Y direction */
    }
    glRotatef(rot, 0.0f, 1.0f, 0.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    OpenCSG::render(primitives);
    OpenCSG::render(primitives2);
    OpenCSG::render(primitives3);
    glDepthFunc(GL_EQUAL);
    std::vector<OpenCSG::Primitive*>::const_iterator itr;
    for (itr = primitives.begin(); itr != primitives.end(); ++itr) {
        (*itr)->render();
    }
    for (itr = primitives2.begin(); itr != primitives2.end(); ++itr) {
        (*itr)->render();
    }
    for (itr = primitives3.begin(); itr != primitives3.end(); ++itr) {
        (*itr)->render();
    }
    glDepthFunc(GL_LESS);

    if (!benchmode)
        renderfps();

    glutSwapBuffers();
}

namespace
{
    const std::string ftransformShader =
        "#version 110\n"
        "void main() {\n"
        "    gl_Position = ftransform();\n"
        "}\n";

    const std::string mvpCombinedShader =
        "#version 110\n"
        "void main() {\n"
        "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
        "}\n";

    const std::string mvpSeparateShader =
        "#version 110\n"
        "void main() {\n"
        "    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;\n"
        "}\n";
}

void menu(int value) {
    switch (value) {
    case CSG_BASIC:      setBasicShape();    break;
    case CSG_WIDGET:     setWidget();        break;
    case CSG_GRID2D:     setGrid2D();        break;
    case CSG_GRID3D:     setGrid3D();        break;
    case CSG_CUBERACK:   setCubeRack();      break;
    case CSG_PIPE:       setPipe();          break;
    case CSG_CONCAVE:    setConcave();       break;

    case ALGO_AUTOMATIC:
        OpenCSG::setOption(OpenCSG::AlgorithmSetting, OpenCSG::Automatic);
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
    case CAM_OUTSIDE_DEFAULT: inside = false;
        OpenCSG::setOption(OpenCSG::CameraOutsideOptimization, OpenCSG::OptimizationDefault);
        break;
    case CAM_OUTSIDE_PERF: inside = false;
        OpenCSG::setOption(OpenCSG::CameraOutsideOptimization, OpenCSG::OptimizationOn);
        break;
    case CAM_OUTSIDE_NOPERF: inside = false;
        OpenCSG::setOption(OpenCSG::CameraOutsideOptimization, OpenCSG::OptimizationOff);
        break;
    case CAM_INSIDE:     inside = true;
        OpenCSG::setOption(OpenCSG::CameraOutsideOptimization, OpenCSG::OptimizationOff);
        break;
    case CAM_INSIDE_DEFAULT: inside = true;
        OpenCSG::setOption(OpenCSG::CameraOutsideOptimization, OpenCSG::OptimizationDefault);
        break;
    case GLSL_FIXED_FUNCTION:
        OpenCSG::setVertexShader("");
        break;
    case GLSL_FTRANSFORM:
        OpenCSG::setVertexShader(ftransformShader);
        break;
    case GLSL_MVP_COMBINED:
        OpenCSG::setVertexShader(mvpCombinedShader);
        break;
    case GLSL_MVP_SEPARATE:
        OpenCSG::setVertexShader(mvpSeparateShader);
        break;
    default: break;
    }
    display();
}

void printNewBenchTableHeadline()
{
    const char* optionName = "";
    switch (benchPerfOption)
    {
    case CAM_OUTSIDE_DEFAULT:
        optionName = "Default performance";
        break;
    case CAM_OUTSIDE_PERF:
        optionName = "Fast performance (at the expense of compatibility)";
        break;
    case CAM_OUTSIDE_NOPERF:
        optionName = "Slower performance (but more compatible)";
        break;
    }
    fprintf(stdout, "\n%s", optionName);
    fprintf(stdout, "\n           Goldfeather    DC         OQ        SCS         DC         OQ");
}

void printNewBenchLine()
{
    const char* shapeName = "";
    switch (benchShape)
    {
    case CSG_BASIC:
        shapeName = "  Simple";
        break;
    case CSG_WIDGET:
        shapeName = "  Widget";
        break;
    case CSG_GRID2D:
        shapeName = " 2D-Grid";
        break;
    case CSG_GRID3D:
        shapeName = " 3D-Grid";
        break;
    case CSG_CUBERACK:
        shapeName = "Cuberack";
        break;
    case CSG_PIPE:
        shapeName = "    Pipe";
        break;
    case CSG_CONCAVE:
        shapeName = " Concave";
        break;
    }
    fprintf(stdout, "\n%s:", shapeName);
}

void applyBenchSetting()
{
    menu(benchAlgorithm);
    menu(benchShape);
    menu(benchPerfOption);
    benchSettingFrameOne = true;
}

void nextBenchSetting()
{
    if (benchShape == BENCH_START)
    {
        rot = 0.0f;
        benchShape = CSG_BASIC;
        benchAlgorithm = GF_STANDARD;
        benchPerfOption = CAM_OUTSIDE_DEFAULT;
        printNewBenchTableHeadline();
        printNewBenchLine();
        applyBenchSetting();
        return;
    }

    ++benchAlgorithm;
    if (   (benchShape != CSG_CONCAVE && benchAlgorithm <= SCS_OQ)
        || (benchShape == CSG_CONCAVE && benchAlgorithm <= GF_OQ))
    {
        applyBenchSetting();
        return;
    }

    benchAlgorithm = GF_STANDARD;
    ++benchShape;
    if (benchShape <= CSG_CONCAVE)
    {
        printNewBenchLine();
        applyBenchSetting();
        return;
    }
    fprintf(stdout, "\n");

    benchShape = CSG_BASIC;
    ++benchPerfOption;
    if (benchPerfOption <= CAM_OUTSIDE_NOPERF)
    {
        printNewBenchTableHeadline();
        printNewBenchLine();
        applyBenchSetting();
        return;
    }

    benchmode = false;
}

void idle() {

    static int ancient = 0;
    static int last = 0;
    static int msec = 0;
    static int numFramesRendered = 0;

    last = msec;
    msec = glutGet(GLUT_ELAPSED_TIME);
    if (spin) {
        rot += static_cast<float>(msec-last) / 10.0f;
    }

    if (benchSettingFrameOne)
    {
        rot = 0;
        benchSettingFrameOne = false;
        last = msec;
        ancient = msec;
        numFramesRendered = 0;
    }

    if (rot >= 360.0f)
    {
        while (rot >= 360.0f)
            rot -= 360.0f;

        if (benchmode)
        {
            float correctedFps = static_cast<float>(numFramesRendered) * 1000.0f / static_cast<float>(msec - ancient);
            fprintf(stdout, "% 11.2f", correctedFps);
            nextBenchSetting();

            rot = 0.0f;
            ancient = msec;
            numFramesRendered = 0;
        }
    }

    if (!benchmode)
    {
        if (last / 1000 != msec / 1000) {

            float correctedFps = static_cast<float>(numFramesRendered) * 1000.0f / static_cast<float>(msec - ancient);
            fpsStream.str("");
            fpsStream << "fps: " << correctedFps << std::ends;

            ancient = msec;
            numFramesRendered = 0;
        }
    }

    display();

    ++numFramesRendered;
}

void key(unsigned char k, int, int) {
    switch (k) {
    case ' ':
        spin = !spin;
        break;
    case 'b':
        benchmode = true;
        benchShape = BENCH_START;
        nextBenchSetting();
        fpsStream.str("");
        break;
    default:
        break;
    }
    display();
}

void init()
{
    // gray background
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

    // Enable two OpenGL lights
    GLfloat light_diffuse[]   = { 1.0f,  0.0f,  0.0f,  1.0f};  // Red diffuse light
    GLfloat light_position0[] = {-1.0f, -1.0f, -1.0f,  0.0f};  // Infinite light location
    GLfloat light_position1[] = { 1.0f,  1.0f,  1.0f,  0.0f};  // Infinite light location

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
    gluPerspective(40.0, 1.0, 0.2, 10.0);
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

    int menuShape     = glutCreateMenu(menu);
    glutAddMenuEntry("Simple",   CSG_BASIC);
    glutAddMenuEntry("Widget",   CSG_WIDGET);
    glutAddMenuEntry("2D-Grid",  CSG_GRID2D);
    glutAddMenuEntry("3D-Grid",  CSG_GRID3D);
    glutAddMenuEntry("Cuberack", CSG_CUBERACK);
    glutAddMenuEntry("Pipe",     CSG_PIPE);
    glutAddMenuEntry("Concave",  CSG_CONCAVE);

    int menuAlgorithm = glutCreateMenu(menu);
    glutAddMenuEntry("Automatic", ALGO_AUTOMATIC);
    glutAddMenuEntry("Goldfeather standard", GF_STANDARD);
    glutAddMenuEntry("Goldfeather depth complexity sampling", GF_DC);
    glutAddMenuEntry("Goldfeather occlusion query", GF_OQ);
    glutAddMenuEntry("SCS standard", SCS_STANDARD);
    glutAddMenuEntry("SCS depth complexity sampling", SCS_DC);
    glutAddMenuEntry("SCS occlusion query", SCS_OQ);

    int menuCamera = glutCreateMenu(menu);
    glutAddMenuEntry("Camera outside (default performance optimizations)", CAM_OUTSIDE_DEFAULT);
    glutAddMenuEntry("Camera outside (with performance optimizations)", CAM_OUTSIDE_PERF);
    glutAddMenuEntry("Camera outside (without performance optimizations)", CAM_OUTSIDE_NOPERF);
    glutAddMenuEntry("Camera inside (default performance optimizations)", CAM_INSIDE_DEFAULT);
    glutAddMenuEntry("Camera inside (without performance optimizations)", CAM_INSIDE);

    int menuGLSL = glutCreateMenu(menu);
    glutAddMenuEntry("Fixed function", GLSL_FIXED_FUNCTION);
    glutAddMenuEntry("ftransform()", GLSL_FTRANSFORM);
    glutAddMenuEntry("gl_ModelViewProjectionMatrix * gl_Vertex", GLSL_MVP_COMBINED);
    glutAddMenuEntry("gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex", GLSL_MVP_SEPARATE);

    glutCreateMenu(menu);
    glutAddSubMenu("CSG Shapes", menuShape);
    glutAddSubMenu("CSG Algorithms", menuAlgorithm);
    glutAddSubMenu("Camera", menuCamera);
    glutAddSubMenu("Vertex Shader", menuGLSL);

    // connect to right mouse button
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutDisplayFunc(display);
    glutKeyboardFunc(key);

    glutIdleFunc(idle);
    init();
    glutMainLoop();

    return 0;
}
