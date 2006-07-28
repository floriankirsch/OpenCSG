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
// scissorMemo.h 
//
// handles scissor calculations
//

#ifndef __OpenCSG__scissor_memo_h__
#define __OpenCSG__scissor_memo_h__

#include "opencsgConfig.h"
#include "area.h"
#include "channelManager.h"
#include <vector>

namespace OpenCSG {

    class ScissorMemo {
    public:
        /// convinience class for handling scissoring. All areas
        /// are given in normal device coordinates [-1, 1] x [-1, 1].
        ScissorMemo();
        
        /// for each channel, the currently set area can be stored
        void store(Channel);
        /// restores the area for the channel
        void recall(Channel);

        /// enables scissoring of area
        void enable() const;
        /// disables scissoring
        void disable() const;

        /// sets the maximum area of the intersected CSG-primitives
        /// by intersecting the bounding boxes of them.
        void setIntersected(const std::vector<Primitive*>& primitives);

        /// sets the area of primitives whose visilibity is determined next.
        void setCurrent(const std::vector<Primitive*>& primitives);
        /// returns the maximum area of the intersected CSG-primitives
        const NDCArea& getIntersectedArea() const;
        /// returns the maximum area of the intersected CSG-primitives
        /// intersected with the currently used primitives
        const NDCArea& getCurrentArea() const;

    protected:
        /// calculates rendering area by intersecting intersected
        /// and current. This is invoked implicetely.
        void calculateArea();

    private:
        NDCArea mIintersection;
        NDCArea mCurrent;
        NDCArea mArea;
        std::vector<NDCArea> mScissor;
    };

} // namespace OpenCSG

#endif // __OpenCSG__scissor_memo_h__


