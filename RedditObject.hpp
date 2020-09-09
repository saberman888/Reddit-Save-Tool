#pragma once
#include <string>
#include "nlohmann/json.hpp"

// TODO: Going to add more later, but I want to start from basics
class RedditObject
{
public:
	RedditObject() : id(), url(), is_video(false), is_self(false){}
	std::string id;
	std::string url;
	bool is_video;
	bool is_self;
	
	std::string GetAudioUrl();
	std::string GetVideoUrl();

};
