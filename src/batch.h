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
// batch.h 
//
// object space optimization: If possible, several primitives are treated 
// as a single batch
//

#ifndef __OpenCSG__batch_h__
#define __OpenCSG__batch_h__

#include <opencsgConfig.h>
#include <vector>

namespace OpenCSG {

    class Primitive;

    typedef std::vector<Primitive*> Batch;
        // a Batch is a list of primitives that do not overlap in screen space

    class Batcher {
    public:
        Batcher(const std::vector<Primitive*>& primitives);
            // subdivides an array of primitives into batches

        std::vector<Batch>::const_iterator begin() const;
            // returns first batch
        std::vector<Batch>::const_iterator end() const;
            // returns end of batches
        unsigned int size() const;
            // return number of batches

    private:
        std::vector<Batch> batches_;
    };

} // namespace OpenCSG

#endif // __OpenCSG__batch_h__
