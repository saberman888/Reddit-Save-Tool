#pragma once
#include <string>
#include "nlohmann/json.hpp"

// TODO: Going to add more later, but I want to start from basics
class RedditObject
{
public:
	RedditObject() : id(), url(), is_video(false){}
	std::string id;
	std::string url;
	bool is_video;
	
	std::string GetAudioUrl();
	std::string GetVideoUrl();

};
