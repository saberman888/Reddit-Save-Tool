#pragma once
#include <string>
#include <fstream>
#include "nlohmann/json.hpp"
#include <iomanip>
#include <iostream>
#include <vector>
#include <sstream>
#include <istream>
#include <algorithm>


std::string stripfname(std::string src);

// Convert unix epoch time to real date time
std::string to_realtime(long timestamp);
std::vector<std::string> splitString(std::string src, char delim);
std::string ToLower(std::string data);

typedef struct _Comment
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
	bool EnableImages, EnableText, DisableComments, RHA, reverse, VideosEnabled, Verbose, EnableCommentThreads, EnableImgurAlbums;
	int limit;
	std::string username;
	std::vector<std::string> whitelist, blacklist, uwhitelist, ublacklist, dblacklist, dwhitelist;
	Sort sort;
};
