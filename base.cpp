#include "base.hpp"

std::string stripfname(std::string src)
{
	std::string characters[] = { "/", "\\", "?", "%", "*", ":", "|", "\"", "<", ">", ".", "\'", "&", ",", "(", ")","#",";"};
	std::string temp;
	for(char chara : src)
	{
		if(std::string(1,chara) == temp)
			continue;
		temp += chara;
	}
	return temp;
}

std::vector<std::string> splitString(std::string src, char delim)
{
	std::vector<std::string> words;
	std::stringstream ss(src);
	std::string temp;
	while (std::getline(ss, temp, delim))
	{
		words.push_back(temp);
	}
	return words;
}

std::string ToLower(std::string data)
{
	std::transform(data.begin(), data.end(), data.begin(),
		[](unsigned char c) { return std::tolower(c); });
		return data;
}