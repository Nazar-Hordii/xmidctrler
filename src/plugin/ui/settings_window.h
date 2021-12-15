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

#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

// XMidiCtrl
#include "ImGuiWindow.h"
#include "settings.h"
#include "xplane.h"

namespace xmidictrl {

class settings_window : public ImGuiWindow {
public:
    settings_window(const std::shared_ptr<xplane>& xp, std::shared_ptr<settings> set);
    ~settings_window() override = default;

protected:
    void create_widgets() override;

private:
    std::shared_ptr<settings> m_settings;

    log_level m_log_level;
    bool m_log_midi;
    bool m_show_messages;

    std::string m_path_xplane;
    std::string m_path_plugin;
    std::string m_path_preferences;
    std::string m_path_profiles;
};

} // Namespace xmidictrl

#endif // SETTINGS_WINDOW_H