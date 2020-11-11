#pragma once
#include <map>
#include <vector>
#include <string>
#include <sstream>

namespace RST
{
	std::vector<std::string> splitString(std::string data, char delimeter);
	template<typename T>
	inline bool contains(std::map<T, T>& lhs, std::string rhs)
	{
		return lhs.find(rhs) != lhs.end();
	}
}
