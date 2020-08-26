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


std::string to_realtime(long timestamp)
{
	time_t ts = timestamp;
	char str[255];
#if defined(_MSC_VER)
	struct tm timeinfo;
	localtime_s(&timeinfo, &ts);
	std::strftime(str, 255, "%A, %B %e, %Y %H:%M %p %Z", &timeinfo);
#else
	struct tm* timeinfo = nullptr;
	timeinfo = localtime(&ts);
	std::strftime(str, 255, "%A, %B %e, %Y %H:%M %p %Z", timeinfo);
#endif

	return std::string(str);
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


CMDArgs::CMDArgs() : EnableImages(true), DisableComments(false), EnableText(true), RHA(false), limit(1000), username(""), sort(Subreddit), reverse(false), VideosEnabled(true), Verbose(false), EnableCommentThreads(false), EnableImgurAlbums(true){}
