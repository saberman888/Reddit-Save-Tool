#pragma once
#include <string>
#include <filesystem>
#include "nlohmann/json.hpp"
#include <fstream>

namespace fs = std::filesystem;

typedef enum RedditType
{
    UNKNOWN=1,
    SELFPOST,
    IMAGE,
    VIDEO
}Post;

// TODO: Going to add more later, but I want to start from basics
class RedditObject
{
public:
	RedditObject() : kind(UNKNOWN), URL(), Id(){}
	Post kind;
	std::string URL, Id;
	
	struct
	{
		int height;
		bool IsGif;
	}VideoInfo;
	std::string GetAudioUrl();
	std::string GetVideoUrl();
    
        void MuxVideo(std::string source, std::string dest);
	void Write(fs::path filepath, std::string filename, const std::string& buffer);

};
