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
// batch.cpp
//

#include <opencsgConfig.h>
#include "batch.h"
#include "primitiveHelper.h"

namespace OpenCSG {

    Batcher::Batcher(const std::vector<Primitive*>& primitives) { 

        const unsigned int numberOfPrimitives = primitives.size();
        std::vector<int> done(numberOfPrimitives, 0);

        unsigned int i = 0;
        for (std::vector<Primitive*>::const_iterator itr = primitives.begin(); itr != primitives.end(); ++itr, ++i) {
            // primitive has already been handled?
            if (done[i]) continue;
            done[i] = 1;

            Batch batch;
            batch.push_back(*itr);

            std::vector<Primitive*>::const_iterator batchCandidate = itr;
            unsigned int j=i+1;
            for (++batchCandidate; batchCandidate != primitives.end(); ++batchCandidate, ++j) {            
                // omit candidate if candidate is already contained in another batch
                if (done[j]) continue;

                Batch::const_iterator currentPrimitives;
                for (currentPrimitives = batch.begin(); currentPrimitives != batch.end(); ++currentPrimitives) {
                    if (Algo::intersectXY(*batchCandidate, *currentPrimitives)) {
                        break;
                    }
                }
                if (currentPrimitives == batch.end()) {
                    batch.push_back(*batchCandidate);
                    done[j] = 1;
                }
            }

            batches_.push_back(batch);
        }        
    }

    std::vector<Batch>::const_iterator Batcher::begin() const {
        return batches_.begin();
    }

    std::vector<Batch>::const_iterator Batcher::end() const {
        return batches_.end();
    }

    unsigned int Batcher::size() const {
        return batches_.size();
    }

} // namespace OpenCSG

