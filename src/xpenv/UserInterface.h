//---------------------------------------------------------------------------------------------------------------------
//   MIT License
//
//   Copyright (c) 2021 Marco Auer
//
//   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
//   documentation files (the "Software"), to deal in the Software without restriction, including without limitation
//   the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
//   to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//   The above copyright notice and this permission notice shall be included in all copies or substantial portions of
//   the Software.
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//   THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
//   CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//   IN THE SOFTWARE.
//---------------------------------------------------------------------------------------------------------------------

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

// Standard
#include <atomic>
#include <string>
#include <vector>

// X-Plane SDK
#include "XPLMDisplay.h"

namespace XPEnv {

    class UserInterface {
    public:
        UserInterface();
        ~UserInterface();

        void createWindow(const std::string& title, int width, int height);

    private:
        void initialise();

        void onDraw(XPLMWindowID id);
        bool onClick(XPLMWindowID id, int x, int y, XPLMMouseStatus status);
        bool onRightClick(XPLMWindowID id, int x, int y, XPLMMouseStatus status);

        XPLMCursorStatus onCursor(XPLMWindowID id, int x, int y);

        bool onMouseWheel(XPLMWindowID id, int x, int y, int wheel, int clicks);

        std::vector<uint32_t> m_buffer;

        int m_width;
        int m_height;
    };

} // Namespace XPEnv

#endif // Namespace USERINTERFACE_H