#include "base.hpp"

std::string stripfname(std::string src)
{
	std::string characters[] = { "/", "\\", "?", "%", "*", ":", "|", "\"", "<", ">", ".", "\'", "&", ",", "(", ")","#",";"};
	for (std::string elem : characters)
	{
		src.erase(
			std::remove(src.begin(), src.end(), elem),
			src.end()
		);
	}
	return src;
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


CMDArgs::CMDArgs() : EnableImages(true), DisableComments(false), EnableText(true), RHA(false), limit(1000), username(""), sort(Subreddit), reverse(false), VideosEnabled(true), Verbose(false), EnableCommentThreads(false), EnableImgurAlbums(true){}
