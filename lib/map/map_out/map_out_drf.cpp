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

#include "map_out_drf.h"

// Standard
#include <utility>

// XMidiCtrl
#include "toml_utils.h"

namespace xmidictrl {

//---------------------------------------------------------------------------------------------------------------------
//   CONSTRUCTOR / DESTRUCTOR
//---------------------------------------------------------------------------------------------------------------------

/**
 * Constructor
 */
map_out_drf::map_out_drf(environment& in_env) : map_out(in_env)
{}


/**
 * Destructor
 */
map_out_drf::~map_out_drf()
{
    m_datarefs.clear();

    m_xp_values.clear();

    m_values_on.clear();
    m_values_off.clear();
}




//---------------------------------------------------------------------------------------------------------------------
//   PUBLIC
//---------------------------------------------------------------------------------------------------------------------

/**
 * Return the mapping type
 */
map_out_type map_out_drf::type()
{
    return map_out_type::dataref;
}


/**
 * Set single value dataref
 */
void map_out_drf::set_dataref(std::string_view in_dataref)
{
    m_datarefs.clear();
    m_datarefs.emplace_back(in_dataref.data());
}


/**
 * Set multiple datarefs
 */
void map_out_drf::set_dataref(std::vector<std::string> in_dataref)
{
    m_datarefs = std::move(in_dataref);
}


/**
 * Set data 2 on
 */
void map_out_drf::set_data_2_on(unsigned char in_data_2_on)
{
    if (in_data_2_on <= MIDI_DATA_2_MAX)
        m_data_2_on = in_data_2_on;
    else
        m_data_2_on = MIDI_DATA_2_MAX;
}


/**
 * Set data 2 off
 */
void map_out_drf::set_data_2_off(unsigned char in_data_2_off)
{
    if (in_data_2_off <= MIDI_DATA_2_MAX)
        m_data_2_off = in_data_2_off;
    else
        m_data_2_off = MIDI_DATA_2_MIN;
}


/**
 * Read settings from config
 */
void map_out_drf::read_config(text_logger& in_log, toml::value& in_data)
{
    in_log.debug_line(in_data.location().line(), "Read settings for type 'drf'");

    read_common_config(in_log, in_data);

    // read dataref
    if (toml_utils::contains(in_log, in_data, CFG_KEY_DATAREF)) {
        // check if single value or array
        if (in_data[CFG_KEY_DATAREF].is_array())
            set_dataref(toml_utils::read_str_vector_array(in_log, in_data, CFG_KEY_DATAREF));
        else
            set_dataref(toml_utils::read_string(in_log, in_data, CFG_KEY_DATAREF));
    }

    // read values on
    if (toml_utils::is_array(in_log, in_data, CFG_KEY_VALUE_ON)) {
        m_values_on = toml_utils::read_str_set_array(in_log, in_data, CFG_KEY_VALUE_ON);
    } else {
        m_values_on.clear();
        std::string value = toml_utils::read_string(in_log, in_data, CFG_KEY_VALUE_ON);

        if (!value.empty())
            m_values_on.insert(value);
    }

    // read values off
    if (toml_utils::is_array(in_log, in_data, CFG_KEY_VALUE_OFF)) {
        m_values_off = toml_utils::read_str_set_array(in_log, in_data, CFG_KEY_VALUE_OFF);
    } else {
        m_values_off.clear();
        std::string value = toml_utils::read_string(in_log, in_data, CFG_KEY_VALUE_OFF);

        if (!value.empty())
            m_values_off.insert(value);
    }

    // read data 2 on
    set_data_2_on(toml_utils::read_midi_value(in_log, in_data, c_cfg_data_2_on, MIDI_DATA_2_MAX));

    // read data 2 off
    set_data_2_off(toml_utils::read_midi_value(in_log, in_data, c_cfg_data_2_off, MIDI_DATA_2_MIN));

    // read send on
    // TODO - Default value in device (or XMidiCtrl)
    if (toml_utils::contains(in_log, in_data, c_cfg_send_on)) {
        if (toml_utils::read_string(in_log, in_data, c_cfg_send_on) == "all")
            m_send_on = send_mode::all;
    }

    // read send off
    // TODO - Default value in device (or XMidiCtrl)
    if (toml_utils::contains(in_log, in_data, c_cfg_send_off)) {
        if (toml_utils::read_string(in_log, in_data, c_cfg_send_off) == "one")
            m_send_off = send_mode::one;
    }
}


/**
 * Check the mapping
 */
bool map_out_drf::check(text_logger& in_log)
{
    bool result = true;

    if (!map::check(in_log))
        result = false;

    if (m_datarefs.empty()) {
        in_log.error(source_line());
        in_log.error(" --> Parameter '" + std::string(CFG_KEY_DATAREF) + "' is not defined");
        result = false;
    }

    if (m_values_on.empty() && m_values_off.empty()) {
        in_log.error(source_line());
        in_log.error(" --> Parameters '" + std::string(CFG_KEY_VALUE_ON) + "' and '" + std::string(CFG_KEY_VALUE_OFF)
                     + "' are not defined");
        result = false;
    }

    for (const auto& dataref: m_datarefs) {
        if (!env().drf().check(dataref)) {
            in_log.error(source_line());
            in_log.error(" --> Dataref '" + std::string(dataref) + "' not found");
            result = false;
        }
    }

    if (m_data_2_on < MIDI_DATA_2_MIN || m_data_2_on > MIDI_DATA_2_MAX) {
        in_log.error(source_line());
        in_log.error(" --> Invalid value for parameter '" + std::string(c_cfg_data_2_on) + "', "
                     + "it has to be between " + std::to_string(MIDI_DATA_2_MIN) + " and "
                     + std::to_string(MIDI_DATA_2_MAX));
        result = false;
    }

    if (m_data_2_off < MIDI_DATA_2_MIN || m_data_2_off > MIDI_DATA_2_MAX) {
        in_log.error(source_line());
        in_log.error(" --> Invalid value for parameter '" + std::string(c_cfg_data_2_off) + "', "
                     + "it has to be between " + std::to_string(MIDI_DATA_2_MIN) + " and "
                     + std::to_string(MIDI_DATA_2_MAX));
        result = false;
    }

    return result;
}


/**
 * Create a MIDI outbound task if required
 */
std::shared_ptr<outbound_task> map_out_drf::execute(text_logger& in_log,
                                                    outbound_send_mode in_send_mode,
                                                    std::string_view in_sl_value)
{
    if (!check_sublayer(in_sl_value))
        return {};

    bool changed = false;

    bool send_msg = false;
    int send_on_cnt = 0;
    int send_off_cnt = 0;

    // if one value has been changed, all other values have to be checked as well
    for (auto& dataref: m_datarefs) {
        // get the current value from X-Plane
        std::string value_current;

        if (!env().drf().read(in_log, dataref, value_current))
            continue;

        // get current value
        std::string value_previous;
        if (m_xp_values.contains(dataref)) {
            value_previous = m_xp_values[dataref];
            m_xp_values[dataref] = value_current;
        } else {
            m_xp_values.emplace(dataref, value_current);
        }

        if (value_current != value_previous)
            changed = true;

        if (in_send_mode == outbound_send_mode::on_change) {
            if (changed)
                send_msg = true;
        } else if (in_send_mode == outbound_send_mode::permanent) {
            send_msg = true;
        }
    }

    if (!send_msg)
        return {};

    // alright, some have been changed, let's check what we have to send out
    for (auto& dataref: m_datarefs) {
        std::string value_current = m_xp_values[dataref];

        // value_on has been defined
        if (!m_values_on.empty()) {
            if (m_values_on.find(value_current) != m_values_on.end()) {
                send_on_cnt++;
                continue;
            } else if (m_values_off.find(value_current) != m_values_off.end() || m_values_off.empty()) {
                send_off_cnt++;
            }
        } else {
            if (m_values_off.find(value_current) != m_values_off.end()) {
                send_off_cnt++;
            } else if (m_values_on.find(value_current) != m_values_on.end() || m_values_on.empty()) {
                send_on_cnt++;
                continue;
            }
        }
    }

    if ((m_send_on == send_mode::all && send_on_cnt == m_datarefs.size())
        || (m_send_on == send_mode::one && send_on_cnt > 0)
        || (m_send_off == send_mode::all && send_off_cnt == m_datarefs.size())
        || (m_send_off == send_mode::one && send_off_cnt > 0)) {
        std::shared_ptr<outbound_task> task = std::make_shared<outbound_task>();

        task->data_changed = changed;

        switch (data_1_type()) {
            case map_data_1_type::control_change:
                task->type = midi_msg_type::control_change;
                break;

            case map_data_1_type::note:
                if ((m_send_on == send_mode::all && send_on_cnt == m_datarefs.size())
                    || (m_send_on == send_mode::one && send_on_cnt > 0))
                    task->type = midi_msg_type::note_on;
                else
                    task->type = midi_msg_type::note_off;
                break;

            case map_data_1_type::pitch_bend:
                task->type = midi_msg_type::pitch_bend;
                break;

            case map_data_1_type::program_change:
                task->type = midi_msg_type::program_change;
                break;

            case map_data_1_type::none:
                task->type = midi_msg_type::none;
                break;
        }

        task->channel = channel();
        task->data_1 = data_1();

        if ((m_send_on == send_mode::all && send_on_cnt == m_datarefs.size())
            || (m_send_on == send_mode::one && send_on_cnt > 0))
            task->data_2 = m_data_2_on;
        else
            task->data_2 = m_data_2_off;


        // TODO: add mapping to task
        //task->mapping = this;

        return task;
    }

    return {};
}


/**
 * Reset the lights on the MIDI device
 */
std::shared_ptr<outbound_task> map_out_drf::reset()
{
    std::shared_ptr<outbound_task> task = std::make_shared<outbound_task>();

    task->data_changed = true;

    switch (data_1_type()) {
        case map_data_1_type::control_change:
            task->type = midi_msg_type::control_change;
            break;

        case map_data_1_type::note:
            task->type = midi_msg_type::note_off;
            break;

        case map_data_1_type::pitch_bend:
            task->type = midi_msg_type::pitch_bend;
            break;

        case map_data_1_type::program_change:
            task->type = midi_msg_type::program_change;
            break;

        case map_data_1_type::none:
            task->type = midi_msg_type::none;
            break;
    }

    task->channel = channel();
    task->data_1 = data_1();
    task->data_2 = MIDI_DATA_2_MIN;

    return task;
}


/**
 * Return mapped dataref
 */
std::string map_out_drf::map_text_drf()
{
    std::string map_str {};

    for (auto& drf: m_datarefs) {
        if (!map_str.empty())
            map_str.append("\n");

        map_str.append(drf);
    }

    return map_str;
}


/**
 * Return mapped parameter
 */
std::string map_out_drf::map_text_parameter()
{
    std::string map_str {};

    // Values on
    if (m_values_on.size() == 1) {
        map_str.append("Value on = " + *m_values_on.begin());
    } else if (m_values_on.size() > 1) {
        map_str.append("Values on = ");

        std::string values_str;
        for (auto& val: m_values_on) {
            if (!values_str.empty())
                values_str.append(", ");

            values_str.append(val);
        }

        map_str.append(values_str);
    }

    if (!map_str.empty() && !m_values_off.empty())
        map_str.append("   |   ");

    // Values off
    if (m_values_off.size() == 1) {
        map_str.append("Value off = " + *m_values_off.begin());
    } else if (m_values_off.size() > 1) {
        map_str.append("Values off = ");

        std::string values_str;
        for (auto& val: m_values_off) {
            if (!values_str.empty())
                values_str.append(", ");

            values_str.append(val);
        }

        map_str.append(values_str);
    }

    // Send modes
    if (m_datarefs.size() > 1) {
        map_str.append("\n");

        if (m_send_on == send_mode::all)
            map_str.append("Send on = all");
        else
            map_str.append("Send on = one");

        map_str.append("   |   ");

        if (m_send_off == send_mode::all)
            map_str.append("Send off = all");
        else
            map_str.append("Send off = one");
    }

    // Data 2 on/off
    if (m_data_2_on != MIDI_DATA_2_MAX)
        map_str.append("   |   Data 2 on = " + std::to_string(m_data_2_on));

    if (m_data_2_off != MIDI_DATA_2_MIN)
        map_str.append("   |   Data 2 off = " + std::to_string(m_data_2_off));

    return map_str;
}




//---------------------------------------------------------------------------------------------------------------------
//   PROTECTED
//---------------------------------------------------------------------------------------------------------------------

/**
 * Return the mapping as string
 */
std::string map_out_drf::build_mapping_text(bool in_short)
{
    std::string map_str {};
    std::string sep_str {"\n"};

    if (!in_short) {
        sep_str = "\n";
        map_str = " ====== Dataref ======" + sep_str;
    }

    // Dataref
    if (m_datarefs.size() == 1) {
        if (!in_short)
            map_str.append("Dataref: ");

        map_str.append(m_datarefs[0]);
    } else {
        if (!in_short)
            map_str.append("Datarefs: ");

        std::string data_str;
        for (auto& str: m_datarefs) {
            if (!data_str.empty())
                data_str.append(" | ");

            data_str.append(str);
        }

        map_str.append(data_str);
    }


    // Values on
    if (m_values_on.size() == 1) {
        map_str.append(sep_str + "Value on: " + *m_values_on.begin());
    } else if (m_values_on.size() > 1) {
        map_str.append(sep_str + "Values on: ");

        std::string values_str;
        for (auto& str: m_values_on) {
            if (!values_str.empty())
                values_str.append(", ");

            values_str.append(str);
        }

        map_str.append(values_str);
    }

    // Values off
    if (m_values_off.size() == 1) {
        map_str.append(sep_str + "Value off: " + *m_values_off.begin());
    } else if (m_values_off.size() > 1) {
        map_str.append(sep_str + "Values off: ");

        std::string values_str;
        for (auto& str: m_values_off) {
            if (!values_str.empty())
                values_str.append(", ");

            values_str.append(str);
        }

        map_str.append(values_str);
    }

    // Data 2 on
    if (m_data_2_on != MIDI_DATA_2_MAX)
        map_str.append(sep_str + "Data 2 on: " + std::to_string(m_data_2_on));

    // Data 2 off
    if (m_data_2_off != MIDI_DATA_2_MIN)
        map_str.append(sep_str + "Data 2 off: " + std::to_string(m_data_2_off));

    // Send on
    if (m_datarefs.size() > 1) {
        if (m_send_on == send_mode::all)
            map_str.append(sep_str + "Send on: 'all'");
        else
            map_str.append(sep_str + "Send on: 'one'");

        // Send off
        if (m_send_off == send_mode::all)
            map_str.append(sep_str + "Send off: 'all'");
        else
            map_str.append(sep_str + "Send off: 'one'");
    }

    return map_str;
}

} // Namespace xmidictrl