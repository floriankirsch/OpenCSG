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
// scissorMemo.cpp
//

#include <opencsgConfig.h>
#include <opencsg.h>
#include <GL/glew.h>
#include "openglHelper.h"
#include "scissorMemo.h"

namespace OpenCSG {

    ScissorMemo::ScissorMemo() : 
        intersection_(Area(-1.0f, -1.0f, 1.0f, 1.0f)),
        current_(Area(1.0f, 1.0f, -1.0f, -1.0f)),
        area_(Area(-1.0f, -1.0f, 1.0f, 1.0f)),
        scissor_(std::vector<Area>(Blue + 1)) {
    }

    void ScissorMemo::store(Channel ch) {
        scissor_[ch] = area_;
    }

    void ScissorMemo::recall(Channel ch) {
        area_ = scissor_[ch];
    }

    void ScissorMemo::enable() const {
        OpenGL::scissor(area_.minx, area_.miny, area_.maxx, area_.maxy);
    }

    void ScissorMemo::disable() const {
        glDisable(GL_SCISSOR_TEST);
    }

    void ScissorMemo::setIntersected(const std::vector<Primitive*>& primitives) {

        float& minx = intersection_.minx;
        float& miny = intersection_.miny;
        float& maxx = intersection_.maxx;
        float& maxy = intersection_.maxy;

        minx = -1.0; miny = -1.0; maxx = 1.0; maxy = 1.0;

        for (std::vector<Primitive*>::const_iterator itr = primitives.begin(); itr != primitives.end(); ++itr) {
            if ((*itr)->getOperation() == Intersection) {
                float tminx, tminy, tminz, tmaxx, tmaxy, tmaxz;
                (*itr)->getBoundingBox(tminx, tminy, tminz, tmaxx, tmaxy, tmaxz);

                minx = std::max(minx, tminx);
                miny = std::max(miny, tminy);
                maxx = std::min(maxx, tmaxx);
                maxy = std::min(maxy, tmaxy);
            }
        }

        minx = std::max(-1.0f, minx);
        minx = std::min( 1.0f, minx);
        miny = std::max(-1.0f, miny);
        miny = std::min( 1.0f, miny);
        maxx = std::max(-1.0f, maxx);
        maxx = std::min( 1.0f, maxx);
        maxy = std::max(-1.0f, maxy);
        maxy = std::min( 1.0f, maxy);

        calculateArea();
    }

    void ScissorMemo::setCurrent(const std::vector<Primitive*>& primitives) {

        float& minx = current_.minx;
        float& miny = current_.miny;
        float& maxx = current_.maxx;
        float& maxy = current_.maxy;

        minx = 1.0; miny = 1.0; maxx = -1.0; maxy = -1.0;

        for (std::vector<Primitive*>::const_iterator itr = primitives.begin(); itr != primitives.end(); ++itr) {
            float tminx, tminy, tminz, tmaxx, tmaxy, tmaxz;
            (*itr)->getBoundingBox(tminx, tminy, tminz, tmaxx, tmaxy, tmaxz);

            minx = std::min(minx, tminx);
            miny = std::min(miny, tminy);
            maxx = std::max(maxx, tmaxx);
            maxy = std::max(maxy, tmaxy);
        }

        minx = std::max(-1.0f, minx);
        minx = std::min( 1.0f, minx); 
        miny = std::max(-1.0f, miny);
        miny = std::min( 1.0f, miny);
        maxx = std::max(-1.0f, maxx);
        maxx = std::min( 1.0f, maxx);
        maxy = std::max(-1.0f, maxy);
        maxy = std::min( 1.0f, maxy);

        calculateArea();
    }

    void ScissorMemo::calculateArea() {
        area_.minx = std::max(current_.minx, intersection_.minx);
        area_.miny = std::max(current_.miny, intersection_.miny);
        area_.maxx = std::min(current_.maxx, intersection_.maxx);
        area_.maxy = std::min(current_.maxy, intersection_.maxy);
    }

} // namespace OpenCSG
