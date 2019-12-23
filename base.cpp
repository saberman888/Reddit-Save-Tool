#include "base.hpp"

std::string stripfname(std::string src)
{
	std::string result = src;
	std::string characters[] = { "/", "\\", "?", "%", "*", ":", "|", "\"", "<", ">", ".", "\'", "&", ",", "(", ")","#",";"};
	for (std::string chr : characters)
		boost::erase_all(result, chr);
	return result;
}

size_t writedat(char* buffer, size_t size, size_t nmemb, std::string& src)
{
	for (size_t i = 0; i < size * nmemb; i++)
	{
		src.push_back(buffer[i]);
	}
	return size * nmemb;
}

std::string to_realtime(long timestamp)
{
	time_t ts = timestamp;
	char str[255];
#if defined(_MSC_VER)
	struct tm timeinfo;
	localtime_s(&timeinfo, &ts);
	std::strftime(str, sizeof(str), "%A, %B %e, %Y %H:%M %p %Z", &timeinfo);
#else
	struct tm* timeinfo = nullptr;
	timeinfo = localtime(&ts);
	std::strftime(str, sizeof(str), "%A, %B %e, %Y %H:%M %p %Z", timeinfo);
#endif

	return std::string(str);
}

std::string get_time(std::string format)
{
    time_t t;
	time(&t);
	char datestr[format.size()];

#if defined(_MSC_VER)
	struct tm timeinfo;
	localtime_s(&timeinfo, &t);
	std::strftime(datestr, sizeof(datestr),format, &timeinfo);
#else
	struct tm* timeinfo = nullptr;
	timeinfo = localtime(&t);
	std::strftime(datestr, sizeof(datestr), format, timeinfo);
#endif
    
    return std::string(datestr);
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

// IsImgurLink only deals with single images whereas IsImgurAlbum deals with albums

bool _Item::IsImgurAlbum()
{
	return (url.rfind("https://imgur.com/a/", 0) != std::string::npos);
}

bool _Item::IsImgurLink()
{
	return (url.rfind("https://imgur.com/", 0) != std::string::npos && url.rfind("https://imgur.com/a/",0) == std::string::npos);
}

State::State() : http_state(0), message("") {}

CMDArgs::CMDArgs() : EnableImages(true), DisableComments(false), EnableText(true), RHA(false), limit(1000), username(""), sort(Subreddit), reverse(false), VideosEnabled(true), Verbose(false), EnableCommentThreads(false), EnableImgurAlbums(true){}
