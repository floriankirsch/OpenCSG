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
// stencilManager.cpp
//

#include <opencsgConfig.h>
#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif
#include "area.h"
#include "stencilManager.h"
#include <vector>

namespace OpenCSG {

    namespace OpenGL {

        StencilManager::StencilManager(const PCArea& area) : area_(area), saved_(false) {}
            // Do not save the stencil buffer at all. 
            // This is, currently, used always.

        StencilManager::~StencilManager() { }

        const PCArea& StencilManager::getArea() const {
            return area_;
        }

        void StencilManager::clear() {
            if (!saved_) {
                save();
                saved_ = true;
            }
            glClear(GL_STENCIL_BUFFER_BIT);
        }

        bool StencilManager::alreadySaved() const {
            return saved_;
        }

        void StencilManager::save() {
            // implemented empty. will not save the stencil buffer
        }

        void StencilManager::restore() {
            // implemented empty. will not restore the stencil buffer
        }





        class StencilManagerGL10 : public StencilManager {
        public:
            StencilManagerGL10(const PCArea& area) : StencilManager(area) {};
                // Saves and restores the stencil buffer by copying it from
                // graphics memory to main memory and back.

                // Warning: There appears to be a bug in this implementation.
                //          Saving and restoring does not work at all.

            virtual void save();
            virtual void restore();
        };

        namespace {
            std::vector<GLubyte>* buf = 0;
            int dx, dy;
        }

        void StencilManagerGL10::save() {
            
            const PCArea& area = getArea();
            dx = area.maxx - area.minx;
            dy = area.maxy - area.miny;

            unsigned int size = (8+dx)*dy; // 8 needed due to possible alignment (?)
            if (!buf) {
                buf = new std::vector<GLubyte>(size);
            } else if (buf->size() < size) {
                buf->resize(size);
            }

            glReadPixels(area.minx, area.miny, dx, dy, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &((*buf)[0]));
        }

        void StencilManagerGL10::restore() {

            if (!alreadySaved()) return; // stencil buffer was never changed, restore nothing

            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            glRasterPos2i(-1, -1);
            glDrawPixels(dx, dy, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &((*buf)[0]));

            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
        }




#ifdef _WIN32
        class StencilManagerARBBufferRegionW32 : public StencilManager {
        public:
            StencilManagerARBBufferRegionW32(const PCArea& area) : StencilManager(area) {};
                // Uses the ARB_buffer_region extension for saving and restoring the 
                // stencil buffer. If this extension is available, this works well
                // and is reasonably fast.

            virtual void save();
            virtual void restore();
        private:
            HDC hdc_; 
            HANDLE handle_;
        };

        void StencilManagerARBBufferRegionW32::save() {

            const PCArea& area = getArea();
            dx = area.maxx - area.minx;
            dy = area.maxy - area.miny;

            hdc_ = wglGetCurrentDC();
            handle_ = wglCreateBufferRegionARB(hdc_, 1, WGL_STENCIL_BUFFER_BIT_ARB);
            if (handle_ == 0) {
                return;
            }
            int result = wglSaveBufferRegionARB(handle_, area.minx, area.miny, dx, dy);

        }

        void StencilManagerARBBufferRegionW32::restore() {

            if (!alreadySaved()) return; // stencil buffer was never changed, restore nothing

            const PCArea& area = getArea();
            dx = area.maxx - area.minx;
            dy = area.maxy - area.miny;

            wglRestoreBufferRegionARB(handle_, area.minx, area.miny, dx, dy, 0, 0);
            wglDeleteBufferRegionARB(handle_);
        }

#endif // _WIN32



        StencilManager* getStencilManager(const PCArea& area) {
#ifdef _WIN32
            /*
            // uncomment for possibility of saving the stencilo buffer under windows
            if (WGLEW_ARB_buffer_region) {
                return new StencilManagerARBBufferRegionW32(area);
            }
            */
#endif // _WIN32

            /*
            // this option has a bug unfortunately and is not usable
            return new StencilManagerGL10(area);
            */
            return new StencilManager(area);
        }

    } // namespace OpenGL

} // namespace OpenCSG


