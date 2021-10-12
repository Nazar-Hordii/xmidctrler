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

#ifndef TYPES_H
#define TYPES_H

// Standard
#include <map>
#include <string>

namespace XMidiCtrl {

//---------------------------------------------------------------------------------------------------------------------
//   CONSTANTS
//---------------------------------------------------------------------------------------------------------------------

// Interval of flight loop
const int FLIGHTLOOP_INTERVAL (-1.0);


// Name of the aircraft profile file
const char* const FILENAME_PROFILE  = "xmidictrl.toml";


// Name of the general settings file
const char* const FILENAME_SETTINGS = "xmidictrl_settings.toml";


// Menu items
const char* const MENUITEM_ABOUT_DIALOG            = "ABOUT_DIALOG";
const char* const MENUITEM_RELOAD_AIRCRAFT_PROFILE = "RELOAD_AIRCRAFT_PROFILE";
const char* const MENUITEM_SEARCH_MIDI_DEVICES     = "SEARCH_MIDI_DEVICES";
const char* const MENUITEM_SETTINGS_DIALOG         = "SETTINGS_DIALOG";
const char* const MENUITEM_SHOW_AIRCRAFT_PROFILE   = "SHOW_AIRCRAFT_PROFILE";


// Keys for the config files
const char* const CFG_KEY_CC                = "CC";
const char* const CFG_KEY_COMMAND           = "command";
const char* const CFG_KEY_COMMAND_BOTTOM    = "command_bottom";
const char* const CFG_KEY_COMMAND_DOWN      = "command_down";
const char* const CFG_KEY_COMMAND_FAST_DOWN = "command_fast_down";
const char* const CFG_KEY_COMMAND_FAST_UP   = "command_fast_up";
const char* const CFG_KEY_COMMAND_PULL      = "command_pull";
const char* const CFG_KEY_COMMAND_PUSH      = "command_push";
const char* const CFG_KEY_COMMAND_TOP       = "command_top";
const char* const CFG_KEY_COMMAND_UP        = "command_up";
const char* const CFG_KEY_DATAREF           = "dataref";
const char* const CFG_KEY_DEVICE            = "device";
const char* const CFG_KEY_NAME              = "name";
const char* const CFG_KEY_PORT_IN           = "port_in";
const char* const CFG_KEY_PORT_OUT          = "port_out";
const char* const CFG_KEY_TYPE              = "type";
const char* const CFG_KEY_VALUE_ON          = "value_on";
const char* const CFG_KEY_VALUE_OFF         = "value_off";
const char* const CFG_KEY_VERSION           = "version";


// Mapping Types as strings
const char* const CFG_MAPTYPE_COMMAND     = "cmd";
const char* const CFG_MAPTYPE_SLIDER      = "sld";
const char* const CFG_MAPTYPE_PUSHANDPULL = "pnp";
const char* const CFG_MAPTYPE_ENCODER     = "enc";
const char* const CFG_MAPTYPE_DATAREF     = "drf";
const char* const CFG_MAPTYPE_INTERNAL    = "int";


// Windows Types
const char* const WINDOW_ABOUT             = "WINDOW_ABOUT";
const char* const WINDOW_DEVICES           = "WINDOW_DEVICES";
const char* const WINDOW_SETTINGS          = "WINDOW_SETTINGS";









//---------------------------------------------------------------------------------------------------------------------
//   TYPES
//---------------------------------------------------------------------------------------------------------------------

// TODO Delete
// List off connected midi devices
//class Device;
//typedef std::map<std::string, std::shared_ptr<Device>> MidiDeviceList;




//---------------------------------------------------------------------------------------------------------------------
//   STRUCTURES
//---------------------------------------------------------------------------------------------------------------------

// TODO Delete
// Midi mapping
//struct MidiMapping {
//    int controlChange;
//    MappingType type;
//
//    std::string command;
//
//    std::string commandPush;
//    std::string commandPull;
//
//    std::string commandUp;
//    std::string commandDown;
//
//    std::string commandUpFast;
//    std::string commandDownFast;
//
//    std::string dataRef;
//    std::string valueOn;
//    std::string valueOff;
//};


// Midi device settings
//struct DeviceSettings {
//    std::string name;
//
//    int portIn;
//    int portOut;
//
//    std::map<int, MidiMapping> mapping;
//};


// Single midi event
// TODO
//class Mapping;
//struct MidiEvent {
//    int status;
//    int controlChange;
//    int velocity;

//    std::shared_ptr<Mapping> mapping;
//};

} // Namespace XMidiCtrl

#endif // TYPES_H
