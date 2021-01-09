#pragma once

#include "nlohmann/json.hpp"
#include <iostream>
#include <string>
namespace SBJSON {
	template<typename T>
	T GetValue(const nlohmann::json& data, std::string tag)
	{
		try {
			return data.at(tag).get<T>();
		}
		catch (nlohmann::json::exception& e) {
			std::cerr << e.what() << std::endl;
		}
		return data.at(tag).get<T>();
	}

	template<typename T>
	bool TryGetValue(const nlohmann::json& data, std::string tag, T& out)
	{
		if (data.contains(tag)) {
			auto jtag = data.at(tag);
			if (jtag.is_null()) {
				out = jtag.get<T>();
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
		return false;
	}

	bool TryGetBool(const nlohmann::json& data, std::string tag);



}
