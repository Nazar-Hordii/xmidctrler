//---------------------------------------------------------------------------------------------------------------------
//   XMidiCtrl - MIDI Controller plugin for X-Plane
//
//   Copyright (c) 2021-2023 Marco Auer
//
//   XMidiCtrl is free software: you can redistribute it and/or modify it under the terms of the
//   GNU Affero General Public License as published by the Free Software Foundation, either version 3
//   of the License, or (at your option) any later version.
//
//   XMidiCtrl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
//   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU Affero General Public License for more details.
//
//   You should have received a copy of the GNU Affero General Public License along with XMidiCtrl.
//   If not, see <https://www.gnu.org/licenses/>.
//---------------------------------------------------------------------------------------------------------------------

#include "map.h"

// XMidiCtrl
#include "conversions.h"

namespace xmidictrl {

//---------------------------------------------------------------------------------------------------------------------
//   CONSTRUCTOR / DESTRUCTOR
//---------------------------------------------------------------------------------------------------------------------

/**
 * Constructor
 */
map::map(environment& in_env)
    : m_env(in_env)
{
}




//---------------------------------------------------------------------------------------------------------------------
//   PUBLIC
//---------------------------------------------------------------------------------------------------------------------

/**
 * Set the mapping number
 */
void map::set_no(unsigned int in_no)
{
    m_no = in_no;
}


/**
 * Return the mapping number
 */
unsigned int map::no() const
{
    return m_no;
}


/**
 * Return the chanel number
 */
unsigned char map::channel() const
{
    return m_channel;
}


/**
 * Return the data type
 */
map_data_1_type map::data_1_type() const
{
    return m_data_1_type;
}


/**
 * Return the data 1 number
 */
unsigned char map::data_1() const
{
    return m_data_1;
}


/**
 * Return data 1 as string
 */
std::string map::data_1_as_string()
{
    std::string str = data_1_type_as_string() + " " + std::to_string(m_data_1);
    return str;
}


/**
 * Return the sublayer name
 */
std::string_view map::sl() const
{
    return m_sl;
}


/**
 * Return the source line
 */
std::string_view map::source_line() const
{
    return m_source_line;
}


/**
 * Return the mapping as text
 */
std::string_view map::map_text(bool in_short)
{
    if (in_short) {
        if (m_map_text_short.empty())
            m_map_text_short = build_mapping_text(true);

        return m_map_text_short;
    } else {
        if (m_map_text_long.empty())
            m_map_text_long = build_mapping_text(false);

        return m_map_text_long;
    }
}


/**
 * Return a string containing channel, type and data
 */
std::string map::get_key()
{
    std::string type_code = data_1_type_as_string();
    return conversions::create_map_key(m_channel, type_code, m_data_1);
}


/**
 * Check the mapping
 */
bool map::check(text_logger&)
{
    if (m_channel != MIDI_NONE && m_data_1 != MIDI_NONE && m_data_1_type != map_data_1_type::none)
        return true;
    else
        return false;
}




//---------------------------------------------------------------------------------------------------------------------
//   PROTECTED
//---------------------------------------------------------------------------------------------------------------------

/**
 * Return the xplane framework
 */
environment& map::env() const
{
    return m_env;
}


/**
 * Read the common config
 */
void map::read_common_config(text_logger& in_log, toml::value& in_data, bool in_read_sl)
{
    // set source line
    m_source_line = std::to_string(in_data.location().line()) + " :: " + in_data.location().line_str();

    // required config
    read_channel(in_log, in_data);
    read_data_1(in_log, in_data);

    // optional config
    if (in_read_sl)
        read_sublayer(in_log, in_data);
}


/**
 * Check if the mapping is defined for the current sublayer
 */
bool map::check_sublayer(std::string_view in_sl_value)
{
    if (in_sl_value != m_sl && !m_sl.empty())
        return false;

    return true;
}




//---------------------------------------------------------------------------------------------------------------------
//   PRIVATE
//---------------------------------------------------------------------------------------------------------------------

/**
 * Read parameter channel
 */
void map::read_channel(text_logger& in_log, toml::value& in_data)
{
    m_channel = 11; // default channel 11

    try {
        // read channel
        if (in_data.contains(c_cfg_ch.data())) {
            m_channel = static_cast<unsigned char>( in_data[c_cfg_ch.data()].as_integer());

            in_log.debug_param(in_data.location().line(), c_cfg_ch, std::to_string(m_channel));
        } else {
            in_log.info(" --> Line " + std::to_string(in_data.location().line()) + " :: "
                        + "Parameter '" + c_cfg_ch.data() + "' is missing, will use default channel '11'");
            in_log.debug_line(in_data.location().line(),
                              "Parameter '" + std::string(c_cfg_ch) + "' = '" + std::to_string(m_channel)
                              + "' (Default Value)");
        }
    } catch (toml::type_error& error) {
        in_log.error_line(in_data.location().line(), "Error reading mapping");
        in_log.error(error.what());
    }
}


/**
 * Read parameter data
 */
void map::read_data_1(text_logger& in_log, toml::value& in_data)
{
    m_data_1 = MIDI_NONE;
    m_data_1_type = map_data_1_type::none;

    try {
        // read control change
        if (in_data.contains(c_cfg_cc.data())) {
            m_data_1 = static_cast<unsigned char>( in_data[c_cfg_cc.data()].as_integer());
            m_data_1_type = map_data_1_type::control_change;

            in_log.debug_line(in_data.location().line(),
                              "Parameter '" + std::string(c_cfg_cc) + "' = '" + std::to_string(m_data_1) + "'");
        } else if (in_data.contains(c_cfg_note.data())) {
            m_data_1 = static_cast<unsigned char>( in_data[c_cfg_note.data()].as_integer());
            m_data_1_type = map_data_1_type::note;

            in_log.debug_line(in_data.location().line(),
                              "Parameter '" + std::string(c_cfg_note) + "' = '" + std::to_string(m_data_1) + "'");
        } else if (in_data.contains(CFG_KEY_PITCH_BEND)) {
            m_data_1 = 0;   // fixed value for pitch bend messages
            m_data_1_type = map_data_1_type::pitch_bend;

            in_log.debug_line(in_data.location().line(),
                              "Parameter '" + std::string(CFG_KEY_PITCH_BEND) + "' = '"
                              + std::to_string(m_data_1) + "' (fixed value for pitch bend)");
        } else if (in_data.contains(CFG_KEY_PROGRAM_CHANGE)) {
            m_data_1 = static_cast<unsigned char>( in_data[CFG_KEY_PROGRAM_CHANGE].as_integer());
            m_data_1_type = map_data_1_type::program_change;

            in_log.debug_line(in_data.location().line(),
                              "Parameter '" + std::string(CFG_KEY_PROGRAM_CHANGE) + "' = '" + std::to_string(m_data_1)
                              + "'");
        } else {
            in_log.error_line(in_data.location().line(), "Parameter for MIDI type is missing");
        }

    } catch (toml::type_error& error) {
        in_log.error_line(in_data.location().line(), "Error reading mapping");
        in_log.error(error.what());
    }
}


/**
 * Read parameter sl
 */
void map::read_sublayer(text_logger &in_log, toml::value &in_data)
{
    m_sl.clear();

    try {
        // read sublayer
        if (in_data.contains(c_cfg_sl.data())) {
            m_sl = in_data[c_cfg_sl.data()].as_string();

            in_log.debug_param(in_data.location().line(), c_cfg_sl, m_sl);
        }
    } catch (toml::type_error &error) {
        in_log.error_line(in_data.location().line(), "Error reading mapping");
        in_log.error(error.what());
    }
}


/**
 * Return the data 1 type as string
 */
std::string map::data_1_type_as_string()
{
    std::string str;

    switch (m_data_1_type) {
        case map_data_1_type::none:
            str = "";
            break;

        case map_data_1_type::control_change:
            str = KEY_CONTROL_CHANGE;
            break;

        case map_data_1_type::note:
            str = KEY_NOTE;
            break;

        case map_data_1_type::pitch_bend:
            str = KEY_PITCH_BEND;
            break;

        case map_data_1_type::program_change:
            str = KEY_PROGRAM_CHANGE;
            break;
    }

    return str;
}

} // Namespace xmidictrl