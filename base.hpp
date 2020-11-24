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
  
  inline bool contains(std::string& lhs, std::string rhs)
	{
		return lhs.find(rhs) != std::string::npos;
	}
  inline bool rcontains(std::string& lhs, std::string rhs)
	{
		return lhs.rfind(rhs) != std::string::npos;
	}
}
