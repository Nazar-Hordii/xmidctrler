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

#include "map_in_enc.h"

// XMidiCtrl
#include "device.h"
#include "toml_utils.h"
#include "conversions.h"

namespace xmidictrl {

//---------------------------------------------------------------------------------------------------------------------
//   CONSTRUCTOR / DESTRUCTOR
//---------------------------------------------------------------------------------------------------------------------

/**
 * Constructor
 */
map_in_enc::map_in_enc(environment &in_env, encoder_mode in_default_enc_mode)
    : map_in(in_env),
      m_mode(in_default_enc_mode)
{}




//---------------------------------------------------------------------------------------------------------------------
//   PUBLIC
//---------------------------------------------------------------------------------------------------------------------

/**
 * Return the mapping type
 */
map_in_type map_in_enc::type()
{
    return map_in_type::encoder;
}


/**
 * Read settings from config
 */
void map_in_enc::read_config(text_logger &in_log, toml::value &in_data, toml::value &in_config)
{
    in_log.debug_line(in_data.location().line(), "Read settings for type 'enc'");
    map_in::read_config(in_log, in_data, in_config);

    // read the mode
    if (toml_utils::contains(in_log, in_data, CFG_KEY_MODE, false))
        m_mode = conversions::encoder_mode_from_code(toml_utils::read_string(in_log, in_data, CFG_KEY_MODE, false));

    // read the delay (if defined)
    if (toml_utils::contains(in_log, in_data, CFG_KEY_DELAY, false))
        m_delay = toml_utils::read_int(in_log, in_data, CFG_KEY_DELAY, false);

    // check if dataref was defined
    if (toml_utils::contains(in_log, in_data, CFG_KEY_DATAREF, false)) {
        in_log.debug_line(in_data.location().line(), "Use 'dataref' mode for encoder mapping");

        // read dataref
        m_dataref = toml_utils::read_string(in_log, in_data, CFG_KEY_DATAREF);

        // read modifier up
        m_modifier_up = toml_utils::read_float(in_log, in_data, CFG_KEY_MODIFIER_UP);

        // read modifier down
        m_modifier_down = toml_utils::read_float(in_log, in_data, CFG_KEY_MODIFIER_DOWN);

        // read modifier fast up
        if (toml_utils::contains(in_log, in_data, CFG_KEY_MODIFIER_FAST_UP, false))
            m_modifier_fast_up = toml_utils::read_float(in_log, in_data, CFG_KEY_MODIFIER_FAST_UP);
        else
            m_modifier_fast_up = m_modifier_up;

        // read modifier fast down
        if (toml_utils::contains(in_log, in_data, CFG_KEY_MODIFIER_FAST_DOWN, false))
            m_modifier_fast_down = toml_utils::read_float(in_log, in_data, CFG_KEY_MODIFIER_FAST_DOWN);
        else
            m_modifier_fast_down = m_modifier_down;

        // read value min
        if (toml_utils::contains(in_log, in_data, CFG_KEY_VALUE_MIN, false)) {
            m_value_min = toml_utils::read_float(in_log, in_data, CFG_KEY_VALUE_MIN, false);
            m_value_min_defined = true;
        }

        // read value max
        if (toml_utils::contains(in_log, in_data, CFG_KEY_VALUE_MAX, false)) {
            m_value_max = toml_utils::read_float(in_log, in_data, CFG_KEY_VALUE_MAX, false);
            m_value_max_defined = true;
        }
    } else {
        in_log.debug_line(in_data.location().line(), "Use 'command' mode for encoder mapping");

        // read command up
        m_command_up = toml_utils::read_string(in_log, in_data, CFG_KEY_COMMAND_UP);

        // read command down
        m_command_down = toml_utils::read_string(in_log, in_data, CFG_KEY_COMMAND_DOWN);

        // read fast command up
        if (toml_utils::contains(in_log, in_data, CFG_KEY_COMMAND_FAST_UP, false))
            m_command_fast_up = toml_utils::read_string(in_log, in_data, CFG_KEY_COMMAND_FAST_UP);
        else
            m_command_fast_up = m_command_up;

        // read fast command down
        if (toml_utils::contains(in_log, in_data, CFG_KEY_COMMAND_FAST_DOWN, false))
            m_command_fast_down = toml_utils::read_string(in_log, in_data, CFG_KEY_COMMAND_FAST_DOWN);
        else
            m_command_fast_down = m_command_down;
    }
}


/**
 * Check the mapping
 */
bool map_in_enc::check(text_logger &in_log)
{
    bool result = true;

    if (!map::check(in_log))
        result = false;

    if (!m_dataref.empty()) {
        // dataref mode
        if (!env().drf().check(m_dataref)) {
            in_log.error(source_line());
            in_log.error(" --> Dataref '" + std::string(m_dataref) + "' not found");
            result = false;
        }

        if (m_modifier_up == 0.0f && m_modifier_down == 0.0f && m_modifier_fast_up == 0.0f
            && m_modifier_fast_down == 0.0f) {
            in_log.error(source_line());
            in_log.error(" --> Modifiers (up/down) are not defined");
            result = false;
        }

        if (m_value_min_defined && m_value_max_defined && m_value_min >= m_value_max) {
            in_log.error(source_line());
            in_log.error(
                " --> Parameter '" + std::string(CFG_KEY_VALUE_MIN) + "' is expected to be less than parameter '"
                + std::string(CFG_KEY_VALUE_MAX) + "'");
            result = false;
        }
    } else {
        // command mode
        if (m_command_up.empty() && m_command_down.empty() && m_command_fast_up.empty()
            && m_command_fast_down.empty()) {
            in_log.error(source_line());
            in_log.error(" --> Commands (up/down) are not defined");
            result = false;
        }
    }

    return result;
}


/**
 * Execute the action in X-Plane
 */
bool map_in_enc::execute(midi_message &in_msg, std::string_view in_sl_value)
{
    if (!check_sublayer(in_sl_value))
        return true;

    if (m_delay > -1) {
        if (m_delay_counter < m_delay) {
            m_delay_counter++;
            return true;
        } else {
            m_delay_counter = 0;
        }
    }

    if (!m_dataref.empty())
        return execute_dataref(in_msg);
    else
        return execute_command(in_msg);
}


/**
 * Return mapped commands/dataref
 */
std::string map_in_enc::map_text_cmd_drf()
{
    std::string map_str {};

    if (!m_dataref.empty()) {
        map_str.append(m_dataref);
    } else {
        map_str.append(m_command_up + "   (up)");
        map_str.append("\n" + m_command_down + "   (down)");

        if (!m_command_fast_up.empty() && m_command_up != m_command_fast_up)
            map_str.append("\n" + m_command_fast_up + "   (fast up)");

        if (!m_command_fast_down.empty() && m_command_down != m_command_fast_down)
            map_str.append("\n" + m_command_fast_down + "   (fast down)");
    }

    return map_str;
}


/**
 * Return mapped parameter
 */
std::string map_in_enc::map_text_parameter()
{
    std::string map_str {};

    if (!m_dataref.empty()) {
        map_str.append("Modifier up = " + conversions::float_to_string(m_modifier_up));
        map_str.append("   |   ");

        if (m_modifier_fast_up != 0) {
            map_str.append("Modifier up (fast) = " + conversions::float_to_string(m_modifier_fast_up));
            map_str.append("   |   ");
        }

        map_str.append("Modifier down = " + conversions::float_to_string(m_modifier_down));
        map_str.append("   |   ");

        if (m_modifier_fast_down != 0) {
            map_str.append("Modifier down (fast) = " + conversions::float_to_string(m_modifier_fast_down));
            map_str.append("   |   ");
        }
    }

    if (m_mode == encoder_mode::relative)
        map_str.append("Mode = relative");
    else
        map_str.append("Mode = range");

    return map_str;
}




//---------------------------------------------------------------------------------------------------------------------
//   PROTECTED
//---------------------------------------------------------------------------------------------------------------------

/**
 * Return the mapping as string
 */
std::string map_in_enc::build_mapping_text(bool in_short)
{
    std::string map_str {};
    std::string sep_str {", "};

    if (!in_short) {
        sep_str = "\n";
        map_str = " ====== Encoder ======" + sep_str;

        if (!sl().empty())
            map_str.append("Sublayer = '" + std::string(sl()) + "'" + sep_str);
    }

    if (!m_dataref.empty()) {
        map_str.append("Dataref = '" + m_dataref + "'" + sep_str);
        map_str.append("Modifier up = " + conversions::float_to_string(m_modifier_up));

        if (m_modifier_fast_up != 0)
            map_str.append(sep_str + "Modifier up (fast) = " + conversions::float_to_string(m_modifier_fast_up));

        map_str.append(sep_str + "Modifier down = " + conversions::float_to_string(m_modifier_down));

        if (m_modifier_fast_down != 0)
            map_str.append(sep_str + "Modifier down (fast) = " + conversions::float_to_string(m_modifier_fast_down));
    } else {
        map_str.append("Command up = '" + m_command_up + "'" + sep_str);

        if (!m_command_fast_up.empty())
            map_str.append("Command up (fast) = '" + m_command_fast_up + "'" + sep_str);

        map_str.append("Command down = '" + m_command_down + "'");

        if (!m_command_fast_down.empty())
            map_str.append(sep_str + "Command down (fast) = '" + m_command_fast_down + "'");
    }

    if (m_mode == encoder_mode::relative)
        map_str.append(sep_str + "Mode = 'relative'");
    else
        map_str.append(sep_str + "Mode = 'range'");

    return map_str;
}




//---------------------------------------------------------------------------------------------------------------------
//   PRIVATE
//---------------------------------------------------------------------------------------------------------------------

/**
 * Execute in dataref mode
 */
bool map_in_enc::execute_dataref(midi_message &in_msg)
{
    float value = 0.0f;
    float modifier = 0.0f;

    // read current value
    if (!env().drf().read(in_msg.log(), m_dataref, value)) {
        m_velocity_prev = in_msg.data_2();
        return true;
    }

    if (m_mode == encoder_mode::relative) {
        // relative mode
        if (in_msg.data_2() < 64) {
            // Down
            if (in_msg.data_2() < 61) {
                in_msg.log().debug(
                    " --> Modify dataref '" + m_dataref + "' by value '" + std::to_string(m_modifier_fast_down) + "'");
                modifier = m_modifier_fast_down;
            } else {
                in_msg.log().debug(
                    " --> Modify dataref '" + m_dataref + "' by value '" + std::to_string(m_modifier_down) + "'");
                modifier = m_modifier_down;
            }
        } else if (in_msg.data_2() > 64) {
            // Up
            if (in_msg.data_2() > 68) {
                in_msg.log().debug(
                    " --> Modify dataref '" + m_dataref + "' by value '" + std::to_string(m_modifier_fast_up) + "'");
                modifier = m_modifier_fast_up;
            } else {
                in_msg.log().debug(
                    " --> Modify dataref '" + m_dataref + "' by value '" + std::to_string(m_modifier_up) + "'");
                modifier = m_modifier_up;
            }
        }
    } else {
        // range mode
        if (m_velocity_prev == MIDI_NONE) {
            // ignore first message, have to figure out first which way the knob was turned
            m_velocity_prev = in_msg.data_2();
            return true;
        }

        switch (in_msg.data_2()) {
            case MIDI_DATA_2_MIN:
                in_msg.log().debug(
                    " --> Modify dataref '" + m_dataref + "' by value '" + std::to_string(m_modifier_up) + "'");
                modifier = m_modifier_down;

                break;

            case MIDI_DATA_2_MAX:
                in_msg.log().debug(
                    " --> Modify dataref '" + m_dataref + "' by value '" + std::to_string(m_modifier_up) + "'");
                modifier = m_modifier_up;

                break;

            default:
                if ((int) (in_msg.data_2() - m_velocity_prev) > 0) {
                    in_msg.log().debug(
                        " --> Modify dataref '" + m_dataref + "' by value '" + std::to_string(m_modifier_up) + "'");
                    modifier = m_modifier_up;
                } else {
                    in_msg.log().debug(
                        " --> Modify dataref '" + m_dataref + "' by value '" + std::to_string(m_modifier_down) + "'");
                    modifier = m_modifier_down;
                }

                break;
        }
    }

    m_velocity_prev = in_msg.data_2();

    // change and check the value
    value = value + modifier;
    value = check_value_min_max(value, modifier);

    if (env().drf().write(in_msg.log(), m_dataref, value)) {
        try {
            display_label(in_msg.log(), value);
        } catch (std::bad_alloc &ex) {
            in_msg.log().error("Error converting float '" + std::to_string(value) + "' value to string");
            in_msg.log().error(ex.what());
        }
    } else {
        in_msg.log().error("Error changing dataref '" + m_dataref + "' to value '" + std::to_string(value) + "'");
    }

    return true;
}


/**
 * Execute in command mode
 */
bool map_in_enc::execute_command(midi_message &in_msg)
{
    if (m_mode == encoder_mode::relative) {
        // relative mode
        if (in_msg.data_2() < 64) {
            // Down
            if (in_msg.data_2() < 61) {
                in_msg.log().debug(" --> Execute command '" + m_command_fast_down + "'");
                env().cmd().execute(in_msg.log(), m_command_fast_down);
            } else {
                in_msg.log().debug(" --> Execute command '" + m_command_down + "'");
                env().cmd().execute(in_msg.log(), m_command_down);
            }
        } else if (in_msg.data_2() > 64) {
            // Up
            if (in_msg.data_2() > 68) {
                in_msg.log().debug(" --> Execute command '" + m_command_fast_up + "'");
                env().cmd().execute(in_msg.log(), m_command_fast_up);
            } else {
                in_msg.log().debug(" --> Execute command '" + m_command_up + "'");
                env().cmd().execute(in_msg.log(), m_command_up);
            }
        }
    } else {
        // range mode
        switch (in_msg.data_2()) {
            case MIDI_DATA_2_MIN:
                in_msg.log().debug(" --> Execute command '" + m_command_down + "'");
                env().cmd().execute(in_msg.log(), m_command_down);
                break;

            case MIDI_DATA_2_MAX:
                in_msg.log().debug(" --> Execute command '" + m_command_up + "'");
                env().cmd().execute(in_msg.log(), m_command_up);
                break;

            default:
                if ((int) (in_msg.data_2() - m_velocity_prev) > 0) {
                    in_msg.log().debug(" --> Execute command '" + m_command_up + "'");
                    env().cmd().execute(in_msg.log(), m_command_up);
                } else {
                    in_msg.log().debug(" --> Execute command '" + m_command_down + "'");
                    env().cmd().execute(in_msg.log(), m_command_down);
                }
                break;
        }

        m_velocity_prev = in_msg.data_2();
    }

    return true;
}


/**
 * Check if the value if within the min/max range
 */
float map_in_enc::check_value_min_max(float in_value, float in_modifier) const
{
    if (in_modifier < 0) {
        // modifier is negative, so we use the minimum value
        if (m_value_min_defined && in_value < m_value_min)
            return m_value_min;
    } else {
        // modifier is positive, so we have to use the maximum value
        if (m_value_max_defined && in_value > m_value_max)
            return m_value_max;
    }

    return in_value;
}

} // Namespace xmidictrl
