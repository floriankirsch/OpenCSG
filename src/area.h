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
// area.h 
//
// area structs
//

#ifndef __OpenCSG__area_h__
#define __OpenCSG__area_h__

#include "opencsgConfig.h"

namespace OpenCSG {

    /// area in pixel coordinates
    struct PCArea {
        PCArea(
            int minxx = 0, 
            int minyy = 0, 
            int maxxx = 0, 
            int maxyy = 0
        ) : 
            minx(minxx), 
            miny(minyy), 
            maxx(maxxx), 
            maxy(maxyy) 
        { }

        int minx, miny, maxx, maxy;
    };

    /// 3d volume in normal device coordinates
    struct NDCVolume {
        NDCVolume(
            float minxx = 0.0f, 
            float minyy = 0.0f, 
            float minzz = 0.0f, 
            float maxxx = 0.0f, 
            float maxyy = 0.0f,
            float maxzz = 0.0f
        ) : 
            minx(minxx), 
            miny(minyy), 
            minz(minzz), 
            maxx(maxxx), 
            maxy(maxyy),  
            maxz(maxzz)
        { } 

        operator PCArea() const;

        float minx, miny, minz, maxx, maxy, maxz;
    };

} // namespace OpenCSG

#endif // __OpenCSG__area_h__

