#pragma once
#include <string>
#include <fstream>
#include "nlohmann/json.hpp"
#include <iomanip>
#include <iostream>


std::string stripfname(std::string src);

// Convert unix epoch time to real date time
std::string to_realtime(long timestamp);

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
// Item holds items from user's saved
class Item
{
public:
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
	int num_comments, depth;
	bool stickied;

	std::string subreddit;


	std::vector < Comment*> comments;
	//Comment comment;

	bool IsVideo();
	// This function quickly checks if a url is an imaged based off the domain name and file extension if possible
	bool IsImage();
	bool IsImgurLink();
	bool IsImgurAlbum();
}Item;
