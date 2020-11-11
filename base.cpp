#include "base.hpp"

std::vector<std::string> RST::splitString(std::string data, char delimeter)
{
  std::vector<std::string> tokens;
  std::string token;
  std::stringstream input(data);
  while(std::getline(input, token, delimeter))
  {
    tokens.push_back(token);
  }
  return tokens;
}

