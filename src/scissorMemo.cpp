// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2002-2006, Florian Kirsch,
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
// scissorMemo.cpp
//

#include "opencsgConfig.h"
#include <opencsg.h>
#include <GL/glew.h>
#include "openglHelper.h"
#include "scissorMemo.h"

namespace OpenCSG {

    ScissorMemo::ScissorMemo() : 
        mIintersection(NDCArea(-1.0f, -1.0f, 1.0f, 1.0f)),
        mCurrent(NDCArea(1.0f, 1.0f, -1.0f, -1.0f)),
        mArea(NDCArea(-1.0f, -1.0f, 1.0f, 1.0f)),
        mScissor(std::vector<NDCArea>(Blue + 1)) {
    }

    void ScissorMemo::store(Channel ch) {
        mScissor[ch] = mArea;
    }

    void ScissorMemo::recall(Channel ch) {
        mArea = mScissor[ch];
    }

    void ScissorMemo::enable() const {
        OpenGL::scissor(mArea);
    }

    void ScissorMemo::disable() const {
        glDisable(GL_SCISSOR_TEST);
    }

    void ScissorMemo::setIntersected(const std::vector<Primitive*>& primitives) {

        float& minx = mIintersection.minx;
        float& miny = mIintersection.miny;
        float& maxx = mIintersection.maxx;
        float& maxy = mIintersection.maxy;

        const int dx = OpenGL::canvasPos[2] - OpenGL::canvasPos[0];
        const int dy = OpenGL::canvasPos[3] - OpenGL::canvasPos[1];

        const float sx  = 2.0f * (static_cast<float>(OpenGL::scissorPos[0]) / static_cast<float>(dx)) - 1.0f;
        const float sy  = 2.0f * (static_cast<float>(OpenGL::scissorPos[1]) / static_cast<float>(dy)) - 1.0f;
        const float swx = 2.0f * (static_cast<float>(OpenGL::scissorPos[2] + OpenGL::scissorPos[0]) / static_cast<float>(dx)) - 1.0f;
        const float swy = 2.0f * (static_cast<float>(OpenGL::scissorPos[3] + OpenGL::scissorPos[1]) / static_cast<float>(dy)) - 1.0f;

        minx = sx; miny = sy; maxx = swx; maxy = swy;

        for (std::vector<Primitive*>::const_iterator itr = primitives.begin(); itr != primitives.end(); ++itr) {
            if ((*itr)->getOperation() == Intersection) {
                float tminx, tminy, tminz, tmaxx, tmaxy, tmaxz;
                (*itr)->getBoundingBox(tminx, tminy, tminz, tmaxx, tmaxy, tmaxz);

                minx = (std::max)(minx, tminx);
                miny = (std::max)(miny, tminy);
                maxx = (std::min)(maxx, tmaxx);
                maxy = (std::min)(maxy, tmaxy);
            }
        }

        minx = (std::max)(-1.0f, minx);
        minx = (std::min)( 1.0f, minx);
        miny = (std::max)(-1.0f, miny);
        miny = (std::min)( 1.0f, miny);
        maxx = (std::max)(-1.0f, maxx);
        maxx = (std::min)( 1.0f, maxx);
        maxy = (std::max)(-1.0f, maxy);
        maxy = (std::min)( 1.0f, maxy);

        calculateArea();
    }

    const NDCArea& ScissorMemo::getIntersectedArea() const {
        return mIintersection;
    }

    const NDCArea& ScissorMemo::getCurrentArea() const {
        return mArea;
    }

    void ScissorMemo::setCurrent(const std::vector<Primitive*>& primitives) {

        float& minx = mCurrent.minx;
        float& miny = mCurrent.miny;
        float& maxx = mCurrent.maxx;
        float& maxy = mCurrent.maxy;

        minx = 1.0; miny = 1.0; maxx = -1.0; maxy = -1.0;

        for (std::vector<Primitive*>::const_iterator itr = primitives.begin(); itr != primitives.end(); ++itr) {
            float tminx, tminy, tminz, tmaxx, tmaxy, tmaxz;
            (*itr)->getBoundingBox(tminx, tminy, tminz, tmaxx, tmaxy, tmaxz);

            minx = (std::min)(minx, tminx);
            miny = (std::min)(miny, tminy);
            maxx = (std::max)(maxx, tmaxx);
            maxy = (std::max)(maxy, tmaxy);
        }

        minx = (std::max)(-1.0f, minx);
        minx = (std::min)( 1.0f, minx); 
        miny = (std::max)(-1.0f, miny);
        miny = (std::min)( 1.0f, miny);
        maxx = (std::max)(-1.0f, maxx);
        maxx = (std::min)( 1.0f, maxx);
        maxy = (std::max)(-1.0f, maxy);
        maxy = (std::min)( 1.0f, maxy);

        calculateArea();
    }

    void ScissorMemo::calculateArea() {
        mArea.minx = (std::max)(mCurrent.minx, mIintersection.minx);
        mArea.miny = (std::max)(mCurrent.miny, mIintersection.miny);
        mArea.maxx = (std::min)(mCurrent.maxx, mIintersection.maxx);
        mArea.maxy = (std::min)(mCurrent.maxy, mIintersection.maxy);
    }

} // namespace OpenCSG
