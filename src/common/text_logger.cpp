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

#include "text_logger.h"

// Standard
#include <vector>

namespace xmidictrl {

//---------------------------------------------------------------------------------------------------------------------
//   CONSTRUCTOR / DESTRUCTOR
//---------------------------------------------------------------------------------------------------------------------

/**
 * Constructor
 */
text_logger::text_logger(text_logger *in_parent)
    : m_parent(in_parent)
{
}


/**
 * Destructor
 */
text_logger::~text_logger()
{
    if (m_file_stream.is_open())
        m_file_stream.close();
}




//---------------------------------------------------------------------------------------------------------------------
//   PUBLIC
//---------------------------------------------------------------------------------------------------------------------

/**
 * Enable file logging
 */
void text_logger::enabled_file_logging(std::string_view in_path)
{
    if (!in_path.empty()) {
        std::string filename = std::string(in_path) + XMIDICTRL_NAME + LOGFILE_SUFFIX;

        m_file_stream.open(filename, std::ios_base::out | std::ios_base::trunc);
        if (!m_file_stream.is_open())
            error("Failed to open log file '%s'", filename.c_str());
    } else {
        error("Cannot open log file as the give file path is empty");
    }
}


/**
 * Set if debug mode is enabled
 */
void text_logger::set_debug_mode(bool in_mode)
{
    m_debug_mode = in_mode;
}


/**
 * Return if debug mode is enabled
 */
bool text_logger::debug_mode() const
{
    return m_debug_mode;
}


/**
 * Set if info messages should be logged
 */
void text_logger::set_log_info(bool in_mode)
{
    m_log_info = in_mode;
}


/**
 * Return if info messages should be logged
 */
bool text_logger::log_info() const
{
    return m_log_info;
}

/**
 * Set the max message size
 */
void text_logger::set_max_size(int in_size)
{
    m_max_size = in_size;
}


/**
 * Return the max message size
 */
int text_logger::max_size() const
{
    return m_max_size;
}


/**
 * Clear all messages
 */
void text_logger::clear()
{
    m_error_count = 0;
    m_warn_count = 0;

    m_messages.clear();
}


/**
 * Return the number of messages
 */
size_t text_logger::count()
{
    return m_messages.size();
}


/**
 * Return a specific text message
 */
text_log_msg *text_logger::message(int in_index)
{
    return m_messages.at(in_index).get();
}


/**
 * Return a text string of all messages
 */
std::string text_logger::messages_as_text()
{
    if (m_messages.size() == 1)
        return {m_messages.at(0)->text};

    std::string text;
    for (auto &msg : m_messages) {
        if (text.empty()) {
            text = msg->text;
        } else {
            text.append("\n");
            text.append(msg->text);
        }
    }

    return text;
}


/**
 * Return if error messages occurred
 */
bool text_logger::has_errors() const
{
    return (m_error_count > 0);
}


/**
 * Return if warning messages occurred
 */
bool text_logger::has_warnings() const
{
    return (m_warn_count > 0);
}


/**
 * Create a debug message
 */
void text_logger::debug(std::string_view in_text, ...)
{
    std::va_list args;
    va_start(args, in_text);

    create_message(log_level::debug, in_text, args);

    if (m_parent != nullptr)
        m_parent->create_message(log_level::debug, in_text, args);

    va_end(args);
}


/**
 * Create an info message
 */
void text_logger::info(std::string_view in_text, ...)
{
    std::va_list args;
    va_start(args, in_text);

    create_message(log_level::info, in_text, args);

    if (m_parent != nullptr)
        m_parent->create_message(log_level::info, in_text, args);

    va_end(args);
}


/**
 * Create a warn message
 */
void text_logger::warn(std::string_view in_text, ...)
{
    std::va_list args;
    va_start(args, in_text);

    m_warn_count++;
    create_message(log_level::warn, in_text, args);

    if (m_parent != nullptr)
        m_parent->create_message(log_level::warn, in_text, args);

    va_end(args);
}


/**
 * Create an error message
 */
void text_logger::error(std::string_view in_text, ...)
{
    std::va_list args;
    va_start(args, in_text);

    m_error_count++;
    create_message(log_level::error, in_text, args);

    if (m_parent != nullptr)
        m_parent->create_message(log_level::error, in_text, args);

    va_end(args);
}




//---------------------------------------------------------------------------------------------------------------------
//   PRIVATE
//---------------------------------------------------------------------------------------------------------------------

/**
 * Check the current log level
 */
bool text_logger::check_log_level(const log_level in_level) const
{
    switch (in_level) {
        case log_level::error:
        case log_level::warn:
            return true;

        case log_level::info:
            if (m_log_info || m_debug_mode)
                return true;
            break;

        case log_level::debug:
            if (m_debug_mode)
                return true;
            break;
    }

    return false;
}


/**
 * Post a message
 */
void text_logger::create_message(log_level in_level, std::string_view in_text, va_list in_args)
{
    if (check_log_level(in_level)) {
        std::vector<char> formatted_text(std::vsnprintf(nullptr, 0, in_text.data(), in_args) + 1);
        std::vsnprintf(formatted_text.data(), formatted_text.size(), in_text.data(), in_args);

        // add message to internal list and write to file stream
        add_message(in_level, formatted_text.data());
    }
}


/**
 * Add message to internal message list
 */
void text_logger::add_message(log_level in_level, std::string_view in_text)
{
    // get current date time stamp
    time_t t = std::time(nullptr);
    struct tm *ltime = localtime(&t);

    // format into a string
    char datetime_str[32];
    std::strftime(&datetime_str[0], sizeof(datetime_str), "%Y-%m-%d %H:%M:%S", ltime);

    // add message to internal list
    std::shared_ptr<text_log_msg> msg = std::make_shared<text_log_msg>();
    msg->time = datetime_str;
    msg->level = in_level;
    msg->text = in_text;

    if (m_debug_mode) {
        while (m_messages.size() >= m_max_size)
            m_messages.pop_front();
    }

    m_messages.push_back(msg);

    // write message to file stream
    if (m_file_stream.is_open()) {
        m_file_stream << msg->time;

        switch (msg->level) {
            case log_level::error:
                m_file_stream << "   [ERROR]   ";
                break;

            case log_level::warn:
                m_file_stream << "   [WARN]    ";
                break;

            case log_level::info:
                m_file_stream << "   [INFO]    ";
                break;

            case log_level::debug:
                m_file_stream << "   [DEBUG]   ";
                break;
        }

        m_file_stream << msg->text;
        m_file_stream << std::endl;
    }
}

} // Namespace xmidictrl
