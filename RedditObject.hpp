#pragma once
#include <string>
#include <filesystem>
#include "nlohmann/json.hpp"
#include "sbjson.hpp"
#include <fstream>
#include "BasicRequest.hpp"
#include <ctime>
#include <sstream>
#include "Imgur.hpp"

namespace RST
{
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
		RedditObject() : kind(UNKNOWN), URL(), Id(), text(), author(), permalink(), title(), domain(), created_utc(0l), Gallery{0}, Video { 0, false }{}
		Post kind;
		std::string URL, Id, text, author, permalink, title, domain;
    time_t created_utc;
      
    struct
    {
        bool IsGallery;
        std::vector<std::string> Images;
    }Gallery;

    struct Videos
		{
			int height;
			bool IsGif;
    }Video;

    std::string GetAudioUrl();
    std::string GetVideoUrl();
    void MuxVideo(std::string source, std::string dest);

    void Write(fs::path filepath, std::string filename, const std::string& buffer);
    void WriteText(fs::path filepath);
    void Read(nlohmann::json& child);
    void ResolveImgurLinks(std::string ClientId);

	};
}
