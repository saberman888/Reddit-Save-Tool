#pragma once
#include <string>
#include <fstream>
#include "nlohmann/json.hpp"
#include <iomanip>
#include <iostream>
#include <vector>
#include <sstream>
#include <istream>
#include <algorithm>


std::string stripfname(std::string src);

// Convert unix epoch time to real date time
std::vector<std::string> splitString(std::string src, char delim);
std::string ToLower(std::string data);
