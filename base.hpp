#pragma once
#include <string>
#include <fstream>
#include "nlohmann/json.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>

constexpr int POST_LIMIT = 1000;
constexpr int LIMIT_PER_TOKEN = 600;
constexpr int RQ_PER_MINUTE = 60;

inline std::string bool2str(bool x) {
	return (x ? "True" : "False");
}

std::string stripfname(std::string src);
size_t writedat(char* buffer, size_t size, size_t nmemb, std::string& src);

// Convert unix epoch time to real date time
std::string to_realtime(long timestamp);

class State
{
public:
		int http_state;
		std::string message;
		State();
};

typedef struct _com
{
	std::string author;
	std::string body, orig_body;
	long created_utc;
	std::string id;
	std::string link_id;
	std::string parent_id;
	std::string subreddit_id;
	bool stickied;
	int score;
}Comment;

typedef enum sortby
{
	Unsorted,
	Subreddit,
	ID,
	Title
}Sort;

class CMDArgs{
public:
	CMDArgs();
	//bool OnlyImages, OnlyText, EnableImages, Everything, EnableComments, EnableText, rha, xlist;
	bool EnableImages, EnableText, DisableComments, RHA, reverse, VideosEnabled, Verbose;
	int limit;
	std::string username;
	std::vector<std::string> whitelist, blacklist, uwhitelist, ublacklist, dblacklist, dwhitelist;
	Sort sort;
};

struct creds {
	std::string username, password, client_id, secret, user_agent;
};

// Item holds items from user's saved
typedef struct _Item
{
	std::string permalink, rha_permalink;
	std::string author;
	long created_utc;
	std::string body, orig_body, parent_id;
	std::string subreddit_id;
	std::string fullname, id, kind;
	std::string domain, title, self_text, orig_self_text, url, extension, fallback_url, audio_url;

	bool is_self, is_video, is_gif;
	bool over_18;
	//bool retrieved_on;
	int score;
	int num_comments;
	bool stickied;

	std::string subreddit;


	std::vector < Comment*> comments;
	//Comment comment;

	bool IsVideo();
	// This function quickly checks if a url is an imaged based off the domain name, and it is used for stats not to really determine if it seriously is an image
	bool IsPossibleImage();
	bool IsImgurLink();
	bool IsImgurAlbum();
}Item;
