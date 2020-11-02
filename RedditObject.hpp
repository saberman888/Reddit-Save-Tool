#pragma once
#include <string>
#include <filesystem>
#include "nlohmann/json.hpp"
#include <fstream>
#include "BasicRequest.hpp"

namespace fs = std::filesystem;

typedef enum RedditType
{
    UNKNOWN=1,
    SELFPOST,
    LINKPOST,
    VIDEO,
	COMMENT
}Post;

// TODO: Going to add more later, but I want to start from basics
class RedditObject
{
public:
	RedditObject() : kind(UNKNOWN), URL(), Id(), text(), author(), permalink(), created_utc(0l), VideoInfo { 0, false }{}
	Post kind;
	std::string URL, Id, text, author, permalink;
  long created_utc;
	
	struct
	{
		int height;
		bool IsGif;
	}VideoInfo;
	std::string GetAudioUrl();
	std::string GetVideoUrl();
    
  void MuxVideo(std::string source, std::string dest);
	void Write(fs::path filepath, std::string filename, const std::string& buffer);
  void WriteText(fs::path filepath);

};
