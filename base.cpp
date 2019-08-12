#include "base.hpp"

size_t writedat(char* buffer, size_t size, size_t nmemb, std::string& src)
{
	for (size_t i = 0; i < size * nmemb; i++)
	{
		src.push_back(buffer[i]);
	}
	return size * nmemb;
}

std::vector<std::string> split_string(std::string src, const char delimeter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(src);
	while (std::getline(tokenStream, token, delimeter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

bool _Item::IsVideo()
{
	return (is_video || domain.rfind("v.redd.it", 0) != std::string::npos || url.rfind("gifv", 0) != std::string::npos);
}

bool _Item::IsPossibleImage()
{
	std::vector<std::string> urlsnext = { "i.imgur.com", "i.redd.it", ".jpeg", ".bmp", ".png", ".gif", ".jpg", ".tiff" };
	for (std::string elem : urlsnext)
		if (url.rfind(elem) != std::string::npos)
			return true;
	return false;
}

CMDArgs::CMDArgs() : EnableImages(true), DisableComments(false), EnableText(true), RHA(false), limit(250), username("") {}