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
// primitive.cpp
//
// implements Primitive interface (declared in opencsg.h)
//

#include <opencsgConfig.h>
#include <opencsg.h>
#include <cassert>

namespace OpenCSG {
    
    Primitive::Primitive(Operation o, unsigned int c) :
        operation_(o),
        convexity_(c),
        minx_(-1.0),
        miny_(-1.0),
        minz_(-1.0),
        maxx_(1.0),
        maxy_(1.0),
        maxz_(1.0) {
    }

    Primitive::~Primitive() { }

    void Primitive::setOperation(Operation o) {
        operation_ = o;
    }

    Operation Primitive::getOperation() const {
        return operation_;
    }

    void Primitive::setConvexity(unsigned int c) {
        convexity_ = c;
    }

    unsigned int Primitive::getConvexity() const {
        return convexity_;
    }

    void Primitive::setBoundingBox(float minx, float miny, float minz,
                                   float maxx, float maxy, float maxz) {
        assert(minx <= maxx);        
        assert(miny <= maxy);        
        assert(minz <= maxz);
        minx_ = minx;
        miny_ = miny;
        minz_ = minz;
        maxx_ = maxx;
        maxy_ = maxy;
        maxz_ = maxz;
    }


    void Primitive::getBoundingBox(float& minx, float& miny, float& minz,
                                   float& maxx, float& maxy, float& maxz) const {
        minx = minx_;
        miny = miny_;
        minz = minz_;
        maxx = maxx_;
        maxy = maxy_;
        maxz = maxz_;
    }

} // namespace OpenCSG
