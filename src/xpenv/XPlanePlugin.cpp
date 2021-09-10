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

// X-Plane SDK
#include "XPLMUtilities.h"

// X-Plane Environment
#include "PluginLogger.h"
#include "PluginLogEntry.h"
#include "XPlanePlugin.h"

namespace XPEnv {

//---------------------------------------------------------------------------------------------------------------------
//   CONSTRUCTOR / DESTRUCTOR
//---------------------------------------------------------------------------------------------------------------------

/**
 * Constructor
 */
XPlanePlugin::XPlanePlugin(std::string_view name, std::string_view version) {
    m_name    = name;
    m_version = version;

    m_pluginId = -1;

    // create environment (link to X-Plane)
    m_environment = std::make_shared<Environment>();

    // initialize the plugin
    initialise();
}




//---------------------------------------------------------------------------------------------------------------------
//   PUBLIC
//---------------------------------------------------------------------------------------------------------------------

/**
 * Return the X-Plane Environment
 */
std::shared_ptr<Environment> XPlanePlugin::environment() {
    return m_environment;
}




//---------------------------------------------------------------------------------------------------------------------
//   PRIVATE
//---------------------------------------------------------------------------------------------------------------------

/** 
 * Initialise the plugin
 */
void XPlanePlugin::initialise() {
    // get plugin id
    m_pluginId = XPLMGetMyID();

    // get plugin path
    XPLMGetPluginInfo(m_pluginId, NULL, m_pluginPath, NULL, NULL);

    // get X-Plane path
    XPLMGetSystemPath(m_xplanePath);

    // initialize logger
    PluginLogger::Instance().initialise(m_xplanePath, m_name);
    
    LOG_INFO << "Plugin " << m_name << " " << m_version << " loaded successfully" << LOG_END
}

} // Namespace XPEnv