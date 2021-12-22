//---------------------------------------------------------------------------------------------------------------------
//   XMidiCtrl - MIDI Controller plugin for X-Plane
//
//   Copyright (c) 2021-2022 Marco Auer
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

#include "map_in_pnp.h"

// XMidiCtrl
#include "logger.h"
#include "utils.h"

namespace xmidictrl {

//---------------------------------------------------------------------------------------------------------------------
//   CONSTRUCTOR / DESTRUCTOR
//---------------------------------------------------------------------------------------------------------------------

/**
 * Constructor
 */
map_in_pnp::map_in_pnp(std::shared_ptr<xplane> xp)
    : map_in(std::move(xp))
{}




//---------------------------------------------------------------------------------------------------------------------
//   PUBLIC
//---------------------------------------------------------------------------------------------------------------------

/**
 * Return the mapping type
 */
map_type map_in_pnp::type()
{
    return map_type::push_pull;
};


/**
 * Set the push command
 */
void map_in_pnp::set_command_push(std::string_view command)
{
    m_command_push = command;
}


/**
 * Return the push command
 */
std::string_view map_in_pnp::command_push() const
{
    return m_command_push;
}


/**
 * Set the pull command
 */
void map_in_pnp::set_command_pull(std::string_view command)
{
    m_command_pull = command;
}


/**
 * Return the pull command
 */
std::string_view map_in_pnp::command_pull() const
{
    return m_command_pull;
}


/**
 * Set the command type
 */
void map_in_pnp::set_command_type(CommandType commandType)
{
    m_command_type = commandType;
}


/**
 * Set the time point when the initial message was received
 */
void map_in_pnp::set_time_point_received()
{
    // reset all times first
    time_point_reset();
    m_time_point_received.store(std::chrono::system_clock::now());
}


/**
 * Set the time point when the initial message was released
 */
void map_in_pnp::set_time_point_released()
{
    // only set the released time, if a received time is present
    if (m_time_point_received.load() > time_point::min())
        m_time_point_released.store(std::chrono::system_clock::now());
}


/**
 * Read settings from config
 */
void map_in_pnp::read_config(toml::value &settings)
{
    LOG_DEBUG << "Line " << settings.location().line() << " :: Read settings for type 'pnp'" << LOG_END
    map::read_config(settings);

    // read command push
    set_command_push(utils::toml_read_string(settings, CFG_KEY_COMMAND_PUSH));

    // read command pull
    set_command_pull(utils::toml_read_string(settings, CFG_KEY_COMMAND_PULL));
}


/**
 * Check the mapping
 */
bool map_in_pnp::check()
{
    if (!map::check())
        return false;

    if (m_command_push.empty() && m_command_pull.empty())
        return false;
    else
        return true;
}


/**
 * Execute the action in X-Plane
 */
bool map_in_pnp::execute(midi_message &msg, std::string_view sl_value)
{
    // Keep it here, in case I need it later again
    /*std::time_t tc = std::chrono::system_clock::to_time_t(m_time_point_released.load());

    char timedisplay[100];
    struct tm buf;
    localtime_s(&buf, &tc);
    std::strftime(timedisplay, sizeof(timedisplay), "%H:%M:%S", &buf);
    LOG_DEBUG << "Time release: " << timedisplay << LOG_END

    tc = std::chrono::system_clock::to_time_t(m_time_point_received.load());
    localtime_s(&buf, &tc);
    std::strftime(timedisplay, sizeof(timedisplay), "%H:%M:%S", &buf);
    LOG_DEBUG << "Time received: " << timedisplay << LOG_END */

    if (!check_sublayer(sl_value) || m_time_point_received.load() == time_point::min()) {
        // wrong sublayer (or received time is missing)
        time_point_reset();
        return true;
    }

    if (m_time_point_released.load() == time_point::min()) {
        // failsafe, don't want to keep this task in the lst forever
        std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - m_time_point_received.load();

        if (elapsed_seconds.count() < 0.5f)
            return false;
    }

    std::chrono::duration<double> elapsed_seconds = m_time_point_released.load() - m_time_point_received.load();

    if (elapsed_seconds.count() > 0.5f || m_time_point_released.load() == time_point::min()) {
        LOG_DEBUG << " --> Execute pull command '" << m_command_pull << "'" << LOG_END
        m_xp->cmd().execute(m_command_pull);
    } else {
        LOG_DEBUG << " --> Execute push command '" << m_command_push << "'" << LOG_END
        m_xp->cmd().execute(m_command_push);
    }

    time_point_reset();
    return true;
}




//---------------------------------------------------------------------------------------------------------------------
//   PRIVATE
//---------------------------------------------------------------------------------------------------------------------

/**
 * Reset the time points to min date
 */
void map_in_pnp::time_point_reset()
{
    m_time_point_received.store(time_point::min());
    m_time_point_released.store(time_point::min());
}

} // Namespace xmidictrl