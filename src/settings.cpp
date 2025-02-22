// OpenCSG - library for image-based CSG rendering for OpenGL
// Copyright (C) 2006-2025, Florian Kirsch
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110 - 1301 USA.

//
// settings.cpp
//

#include "opencsgConfig.h"
#include <opencsg.h>

#include <algorithm>
#include <string>
#include <utility>

namespace OpenCSG {

    static std::string gVertexShader;

    void setVertexShader(const std::string& vertexShader) {
        if (gVertexShader == vertexShader)
            return;

	std::string memoryForNewShader = vertexShader;
	std::swap(gVertexShader, memoryForNewShader);
    }

    const char* getVertexShader()
    {
        if (gVertexShader.length() == 0)
            return 0;

        return gVertexShader.c_str();
    }

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
