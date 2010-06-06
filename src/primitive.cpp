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
// primitive.cpp
//
// implements Primitive interface (declared in opencsg.h)
//

#include "opencsgConfig.h"
#include <opencsg.h>
#include <cassert>

namespace OpenCSG {
    
    Primitive::Primitive(Operation o, unsigned int c) :
        mOperation(o),
        mConvexity(c),
        mMinx(-1.0),
        mMiny(-1.0),
        mMinz(-1.0),
        mMaxx(1.0),
        mMaxy(1.0),
        mMaxz(1.0) {
    }

    Primitive::~Primitive() { }

    void Primitive::setOperation(Operation o) {
        mOperation = o;
    }

    Operation Primitive::getOperation() const {
        return mOperation;
    }

    void Primitive::setConvexity(unsigned int c) {
        mConvexity = c;
    }

    unsigned int Primitive::getConvexity() const {
        return mConvexity;
    }

    void Primitive::setBoundingBox(float minx, float miny, float minz,
                                   float maxx, float maxy, float maxz) {
        assert(minx <= maxx);
        assert(miny <= maxy);
        assert(minz <= maxz);
        mMinx = minx;
        mMiny = miny;
        mMinz = minz;
        mMaxx = maxx;
        mMaxy = maxy;
        mMaxz = maxz;
    }


    void Primitive::getBoundingBox(float& minx, float& miny, float& minz,
                                   float& maxx, float& maxy, float& maxz) const {
        minx = mMinx;
        miny = mMiny;
        minz = mMinz;
        maxx = mMaxx;
        maxy = mMaxy;
        maxz = mMaxz;
    }

} // namespace OpenCSG
