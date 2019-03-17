#pragma once

#include "curl/curl.h"
#include <vector>
#include <string>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iosfwd>
#include <sstream>
#include <map>
#include <boost/algorithm/string.hpp>
#include <stdlib.h>
#include <chrono>
#include <ratio>
#include <iomanip>
#include <vector>

#define POST_LIMIT 1000
#define LIMIT_PER_TOKEN 600
#define RQ_PER_MINUTE 60

const std::string initial_url = "https://www.reddit.com/api/v1/access_token";
#define SCOPE "%20save%20account%20read%20history"
void QFIO(std::string filename, std::string data);
void JQFIO(std::string filename, std::string json);
typedef struct _State
{
	int http_state;
	std::string message;
}State;


typedef struct _Item
{
	bool is_self;
	bool is_comment;
	std::string permalink;
	std::string fullname, id;
	std::string link_url;
	std::string domain;
}Item;



size_t writedat(char* buffer, size_t size, size_t nmemb, std::string& src);
std::map<std::string, std::string> MapHeaders(std::string source);

class Saver
{
public:

	Saver(std::string username, std::string password, std::string client_id, std::string secret, std::string useragent);
	void OldRedditToggle(bool val) { this->use_old_reddit = val; }
	void ListLinkedUrls(bool val) { this->list_linked_urls = val; }
	State Save(std::string fullname);
	State UnSave(std::string fullname);
	State AccessSaved(std::vector<Item*>& saved);
	State AccessReddit() { return this->obtain_token(false); }
	State RefreshToken() { return this->obtain_token(true); }

private:
	std::string username;
	std::string password;
	std::string client_id;
	std::string secret;
	std::string useragent;

	std::string before;
	std::string after;

	int requests_done;
	int request_done_in_current_minute;

	std::chrono::system_clock::time_point now;
	std::chrono::system_clock::time_point then;

	std::chrono::system_clock::time_point mnow;
	std::chrono::system_clock::time_point mthen;

	std::string token;

	State obtain_token(bool refresh);
	State SaveToggle(std::string fullname, bool remove);
	State get_saved_items(std::vector<Item*>& sitem);

	void restart_minute_clock();
	void is_mtime_up();

	bool is_time_up() {if (now >= then) { return true; } return false;}

	bool use_old_reddit;
	bool list_linked_urls;
public:
	bool backup_as_json(std::string filename, std::vector<Item*>& src);
	bool output_simple_format(std::string filename, std::vector<Item*>& src);
};