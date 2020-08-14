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

bool Item::IsVideo()
{
	return (is_video || domain.rfind("https://v.redd.it", 0) != std::string::npos);
}

bool Item::IsImage()
{
	std::vector<std::string> urlsnext = { "i.imgur.com", "i.redd.it", ".jpeg", ".bmp", ".png", ".gif", ".jpg", ".tiff", ".webp" };
	for (std::string elem : urlsnext)
		if (url.rfind(elem) != std::string::npos)
			return true;
	return false;
}

// IsImgurLink only deals with single images whereas IsImgurAlbum deals with albums

bool Item::IsImgurAlbum()
{
	return url.rfind("https://imgur.com/a/", 0) != std::string::npos;
}

bool Item::IsImgurLink()
{
	return url.rfind("https://imgur.com/", 0) != std::string::npos;
}


CMDArgs::CMDArgs() : EnableImages(true), DisableComments(false), EnableText(true), RHA(false), limit(1000), username(""), sort(Subreddit), reverse(false), VideosEnabled(true), Verbose(false), EnableCommentThreads(false), EnableImgurAlbums(true){}
