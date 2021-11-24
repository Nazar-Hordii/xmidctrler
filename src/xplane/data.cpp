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

#include "data.h"

// Standard
#include <sstream>
#include <string>

// XMidiCtrl
#include "logger.h"

namespace xmidictrl {

//---------------------------------------------------------------------------------------------------------------------
//   CONSTRUCTOR / DESTRUCTOR
//---------------------------------------------------------------------------------------------------------------------

/**
 * Destructor
 */
data::~data()
{
    m_data_cache.clear();
}




//---------------------------------------------------------------------------------------------------------------------
//   PUBLIC
//---------------------------------------------------------------------------------------------------------------------

/**
 * Read a dataref
 */
bool data::read(std::string_view name, std::string &value)
{
    value.assign(std::string());

    data_item *item = retrieve_data(name);

    if (item == nullptr)
        return false;

    // check the dataref type
    if (item->type & xplmType_Int) {
        int i = read_int(item);
        value.assign(std::to_string(i));

        return true;

    } else if (item->type & xplmType_Float) {
        std::stringstream ss;

        ss << read_float(item);
        value.assign(ss.str());

        return true;

    } else if (item->type & xplmType_Double) {
        std::stringstream ss;

        ss << read_double(item);
        value.assign(ss.str());

        value.assign(ss.str());

        return true;

    } else if (item->type & xplmType_Data) {
        value.assign(read_byte(item));

        return true;

    } else if (item->type & xplmType_IntArray) {
        LOG_ERROR << "Unsupported type 'IntArray' of dataref '" << name.data() << "'" << LOG_END

        return false;

    } else if (item->type & xplmType_FloatArray) {
        LOG_ERROR << "Unsupported type 'FloatArray' of dataref '" << name.data() << "'" << LOG_END

        return false;

    } else if (item->type & xplmType_Unknown) {
        LOG_ERROR << "Could not determine type of dataref '" << name.data() << "'" << LOG_END

        return false;

    } else {
        LOG_ERROR << "Unknown type '" << item->type << "' for dataref '" << name.data() << "'" << LOG_END

        return false;
    }
}


/**
 * Toggle a dataref between on and off
 */
bool data::toggle(std::string_view name, std::string_view value_on, std::string_view value_off)
{
    data_item *item = retrieve_data(name);

    if (item == nullptr)
        return false;

    if (!item->writeable) {
        LOG_ERROR << "Dataref '" << name.data() << "' is not writeable" << LOG_END
        return false;
    }

    if (item->type & xplmType_Int) {
        toggle_int(item, value_on, value_off);
        return true;
    } else if (item->type & xplmType_Float) {
        toggle_float(item, value_on, value_off);
        return true;
    } else if (item->type & xplmType_Double) {
        toggle_double(item, value_on, value_off);
        return true;
    } else if (item->type & xplmType_Data) {
        toggle_byte(item, value_on, value_off);
        return true;
    } else if (item->type & xplmType_IntArray) {
        LOG_ERROR << "Unsupported type 'IntArray' of dataref '" << name.data() << "'" << LOG_END
        return false;
    } else if (item->type & xplmType_FloatArray) {
        LOG_ERROR << "Unsupported type 'FloatArray' of dataref '" << name.data() << "'" << LOG_END
        return false;
    } else if (item->type & xplmType_Unknown) {
        LOG_ERROR << "Could not determine type of dataref '" << name.data() << "'" << LOG_END
        return false;
    } else {
        LOG_ERROR << "Unknown type '" << item->type << "' for dataref '" << name.data() << "'" << LOG_END
        return false;
    }
}




//---------------------------------------------------------------------------------------------------------------------
//   PRIVATE
//---------------------------------------------------------------------------------------------------------------------

/**
 * Get the data ref for a dataref string
 */
data_item *data::retrieve_data(std::string_view name)
{
    if (m_data_cache.count(name.data()) == 0) {
        std::unique_ptr<data_item> item = std::make_unique<data_item>();

        item->dataref = XPLMFindDataRef(name.data());
        item->type = xplmType_Unknown;

        if (item->dataref == nullptr) {
            LOG_ERROR << "Dataref '" << name.data() << "' not found" << LOG_END
            return {};
        }

        item->type = XPLMGetDataRefTypes(item->dataref);
        item->writeable = XPLMCanWriteDataRef(item->dataref);

        // in case of a byte ref, we have to determine to size
        if (item->type & xplmType_Data)
            item->size = XPLMGetDatab(item->dataref, nullptr, 0, 0);

        // add new dataref to cache
        LOG_DEBUG << "Dataref '" << name.data() << "' added to internal cache" << LOG_END
        m_data_cache.emplace(name, std::move(item));
    }

    return m_data_cache.at(name.data()).get();
}


/**
 * Read a integer dataref
 */
int data::read_int(const data_item *item)
{
    // read current value
    return XPLMGetDatai(item->dataref);
}


/**
 * Read a float dataref
 */
float data::read_float(const data_item *item)
{
    // read current value
    return XPLMGetDataf(item->dataref);
}


/**
 * Read a double dataref
 */
double data::read_double(const data_item *item)
{
    // read current value
    return XPLMGetDatad(item->dataref);
}


/**
 * Read a byte dataref
 */
std::string data::read_byte(const data_item *item)
{
    std::vector<char> tmpValue(item->size);
    XPLMGetDatab(item->dataref, &tmpValue[0], 0, tmpValue.size());

    return {tmpValue.data(), strnlen(tmpValue.data(), tmpValue.size())};
}


/**
 * Toggle an integer dataref between on and off
 */
void data::toggle_int(const data_item *item, std::string_view value_on, std::string_view value_off)
{
    // read current value
    int value = read_int(item);

    if (value == std::stoi(value_on.data())) {
        LOG_DEBUG << "Set dataref '" << item->name << "' to value '" << value_off.data() << "'" << LOG_END
        XPLMSetDatai(item->dataref, std::stoi(value_off.data()));
    } else {
        LOG_DEBUG << "Set dataref '" << item->name << "' to value '" << value_on.data() << "'" << LOG_END
        XPLMSetDatai(item->dataref, std::stoi(value_on.data()));
    }
}


/**
 * Toggle a float dataref between on and off
 */
void data::toggle_float(const data_item *item, std::string_view value_on, std::string_view value_off)
{
    // read current value
    float value = read_float(item);

    if (value == std::stof(value_on.data())) {
        LOG_DEBUG << "Set dataref '" << item->name << "' to value '" << value_off.data() << "'" << LOG_END
        XPLMSetDataf(item->dataref, std::stoi(value_off.data()));
    } else {
        LOG_DEBUG << "Set dataref '" << item->name << "' to value '" << value_on.data() << "'" << LOG_END
        XPLMSetDataf(item->dataref, std::stoi(value_on.data()));
    }
}


/**
 * Toggle an double dataref between on and off
 */
void data::toggle_double(const data_item *item, std::string_view value_on, std::string_view value_off)
{
    // read current value
    double value = read_double(item);

    if (value == std::stod(value_on.data())) {
        LOG_DEBUG << "Set dataref '" << item->name << "' to value '" << value_off.data() << "'" << LOG_END
        XPLMSetDatad(item->dataref, std::stoi(value_off.data()));
    } else {
        LOG_DEBUG << "Set dataref '" << item->name << "' to value '" << value_on.data() << "'" << LOG_END
        XPLMSetDatad(item->dataref, std::stoi(value_on.data()));
    }
}


/**
 * Toggle a nyte dataref between on and off
 */
void data::toggle_byte(const data_item *item, std::string_view value_on, std::string_view value_off)
{
    std::string value = read_byte(item);

    if (value == value_on.data()) {
        LOG_DEBUG << "Set dataref '" << item->name << "' to value '" << value_off.data() << "'" << LOG_END
        XPLMSetDatab(item->dataref, const_cast<char *>(value_off.data()), 0, value_off.size() + 1);
    } else {
        LOG_DEBUG << "Set dataref '" << item->name << "' to value '" << value_on.data() << "'" << LOG_END
        XPLMSetDatab(item->dataref, const_cast<char *>(value_on.data()), 0, value_on.size() + 1);
    }
}

} // Namespace xmidictrl