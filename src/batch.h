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
// batch.h 
//
// object space optimization: If possible, several primitives are treated 
// as a single batch
//

#ifndef __OpenCSG__batch_h__
#define __OpenCSG__batch_h__

#include "opencsgConfig.h"
#include <vector>

namespace OpenCSG {

    class Primitive;

    /// a Batch is a list of primitives that do not overlap in screen space
    typedef std::vector<Primitive*> Batch;

    class Batcher {
    public:
        /// subdivides an array of primitives into batches
        Batcher(const std::vector<Primitive*>& primitives);

        /// returns first batch
        std::vector<Batch>::const_iterator begin() const;
        /// returns end of batches
        std::vector<Batch>::const_iterator end() const;
        /// return number of batches
        unsigned int size() const;

    private:
        std::vector<Batch> mBatches;
    };

} // namespace OpenCSG

#endif // __OpenCSG__batch_h__
