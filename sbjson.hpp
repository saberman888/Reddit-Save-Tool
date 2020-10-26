#pragma once

#include "nlohmann/json.hpp"
#include <string>

template<typename T>
static T get_value(const nlohmann::json& data, std::string tag)
{
    try {
        return data.at(tag).get<T>();
    }
    catch (nlohmann::json::exception& e) {
        std::cerr << "Exception triggered by tag: " << tag << std::endl;
        std::cerr << e.what() << std::endl;
        throw;
    }
}

inline bool get_bool(const nlohmann::json& data, std::string tag)
{
    return get_value<bool>(data, tag);
}

inline std::string get_string(const nlohmann::json& data, std::string tag)
{
    return get_value<std::string>(data, tag);
}

inline int get_int(const nlohmann::json& data, std::string tag)
{
    return get_value<int>(data, tag);
}

inline time_t get_long(const nlohmann::json& data, std::string tag)
{
    return get_value<time_t>(data, tag);
}


inline bool DoesExist(const nlohmann::json& data, std::string tag)
{
    return data.contains(tag);
}
