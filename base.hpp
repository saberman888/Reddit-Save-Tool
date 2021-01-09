#pragma once
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>
#include "BasicRequest.hpp"

namespace RST
{
	std::vector<std::string> splitString(std::string data, char delimeter);
	template<typename T>
	inline bool contains(std::map<T, T>& lhs, std::string rhs)
	{
		return lhs.find(rhs) != lhs.end();
	}

	inline bool contains(std::string& lhs, std::string rhs)
	{
		return lhs.find(rhs) != std::string::npos;
	}
	inline bool rcontains(std::string& lhs, std::string rhs)
	{
		return lhs.rfind(rhs) != std::string::npos;
	}

	bool Write(std::filesystem::path destination, const std::string& buffer);
	State Download(const std::string URL);
	std::string SearchAndReplace(std::string Input, const std::string ToBeReplaced, const std::string Replacement);
}
