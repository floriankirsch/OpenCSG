// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2006, Florian Kirsch
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
// settings.cpp
//

#include "opencsgConfig.h"
#include <opencsg.h>

namespace OpenCSG {

    static int* gSetting = 0;

    void initIntOptions() {
        if (!gSetting) {
            gSetting = new int[OptionTypeUnused];
            for (int i=0; i<OptionTypeUnused; ++i) {
                gSetting[i] = 0;
            }
        }
    }

    void setOption(OptionType option, int newSetting) {
        if ((unsigned int)option < OptionTypeUnused) {
            initIntOptions();
            gSetting[option] = newSetting;
        }
    }

    int getOption(OptionType option) {
        initIntOptions();
        if ((unsigned int)option < OptionTypeUnused) {
            return gSetting[option];
        }

        return 0;
    }

} // namespace OpenCSG
