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
// scissorMemo.h 
//
// handles scissor calculations
//

#ifndef __OpenCSG__scissor_memo_h__
#define __OpenCSG__scissor_memo_h__

#include <opencsgConfig.h>
#include "area.h"
#include "channelManager.h"
#include <vector>

namespace OpenCSG {

    class ScissorMemo {
    public:
        ScissorMemo();
            // convinience class for handling scissoring. All areas
            // are given in normal device coordinates [-1, 1] x [-1, 1].

        void store(Channel);
            // for each channel, the currently set area can be stored
        void recall(Channel);
            // restores the area for the channel

        void enable() const;
            // enables scissoring of area
        void disable() const;
            // disables scissoring

        void setIntersected(const std::vector<Primitive*>& primitives);
            // sets the maximum area of the intersected CSG-primitives
            // by intersecting the bounding boxes of them.
        void setCurrent(const std::vector<Primitive*>& primitives);
            // sets the area of primitives whose visilibity is determined
            // next.
        const NDCArea& getIntersectedArea() const;
            // returns the maximum area of the intersected CSG-primitives
        const NDCArea& getCurrentArea() const;
            // returns the maximum area of the intersected CSG-primitives
            // intersected with the currently used primitives

    protected:
        void calculateArea();
            // calculates rendering area by intersecting intersected
            // and current. This is invoked implicetely.

    private:

        NDCArea intersection_;
        NDCArea current_;
        NDCArea area_;
        std::vector<NDCArea> scissor_;
    };

} // namespace OpenCSG

#endif // __OpenCSG__scissor_memo_h__


