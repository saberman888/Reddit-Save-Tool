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

/*

QFIO and JQFIO output text such as json responses into a file

QFIO is just for normal stuff like the header of a request while JQFIO is specifically for JSON
it parses it in nlohmann json first then pretty prints it to a file, but if it fails it just outputs normally to a file without pretty print

*/
inline void QFIO(std::string filename, std::string data)
{
	std::clog << "Outputting " << filename << std::endl;
	std::ofstream(filename.c_str(), std::ios::out) << data;
}
inline void JQFIO(std::string filename, std::string json)
{
	nlohmann::json  data;
	try {
		data = nlohmann::json::parse(json);

		std::ofstream(filename.c_str(), std::ios::out) << std::setw(4) << data;
	}
	catch (nlohmann::json::parse_error&) {
		std::ofstream(filename.c_str(), std::ios::out) << json;
	}
}
inline std::string bool2str(bool x) {
	return (x ? "True" : "False");
}

std::string stripfname(std::string src);
size_t writedat(char* buffer, size_t size, size_t nmemb, std::string& src);

// Convert unix epoch time to real date time
std::string to_realtime(long timestamp);

typedef struct _State
{
	int http_state;
	std::string message;
}State;

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
	//bool OnlyImages, OnlyText, EnableImages, Everything, EnableComments, EnableText, rha;
	bool EnableImages, EnableText, DisableComments, RHA;
	int limit;
	std::string username;
	std::vector<std::string> whitelist, blacklist;
	Sort sort;
};

struct creds {
	std::string username, password, client_id, secret, user_agent;
};

// Item holds items from user's saved
typedef struct _Item
{
	std::string permalink;
	std::string author;
	long created_utc;
	std::string body, orig_body, parent_id;
	std::string subreddit_id;
	std::string fullname, id, kind;
	std::string domain, title, self_text, orig_self_text, url, extension;

	bool is_self, is_video;
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
}Item;
