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

#ifndef PLUGIN_H
#define PLUGIN_H

// Standard
#include <map>

// X-Plane SDK
#include "XPLMProcessing.h"

// X-Plane Environment
#include "UserInterface.h"
#include "XPlanePlugin.h"

// XMidiCtrl
#include "Device.h"
#include "EventHandler.h"
#include "Menu.h"
#include "Settings.h"
#include "Types.h"

namespace XMidiCtrl {

class Plugin : public XPEnv::XPlanePlugin {
public:
    Plugin();
    ~Plugin();

    static Plugin& Instance();

    static float callbackFlightLoop(float elapsedMe, float elapsedSim, int counter, void* refcon);

    void enablePlugin() override;
    void disablePlugin() override;

    void reloadAircraftSettings();

    void addMidiEvent(const std::shared_ptr<MidiEvent>& midiEvent);

    void showMidiDevicesDialog();
    void showAboutDialog() override;

private:
    void processFlightLoop(float elapsedMe, float elapsedSim, int counter);

    void initialiseDevices();
    void closeMidiConnections();
    void destroyDeviceList();
   
    Menu m_menu;
    EventHandler m_eventHandler;  
    Settings m_settings;
    UserInterface m_ui;

    XPLMFlightLoopID m_flightLoopId;
    MidiDeviceList m_midiDevices;
};

} // Namespace XMidiCtrl

#endif // PLUGIN_H