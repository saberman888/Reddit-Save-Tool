#include "RedditObject.hpp"

std::string RedditObject::GetAudioUrl()
{
	return url + "/DASH_audio.mp4";
}

std::string RedditObject::GetVideoUrl()
{
	return url + "/DASH_480.mp4";
}
