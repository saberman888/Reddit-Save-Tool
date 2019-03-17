#pragma once

#include "curl/curl.h"
#include <vector>
#include <string>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iosfwd>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <stdlib.h>
#include <chrono>
#include <ratio>
#include <iomanip>
#include <vector>

#define POST_LIMIT 1000
#define LIMIT_PER_TOKEN 600
#define RQ_PER_MINUTE 60

#define SCOPE "%20save%20history"

/*

QFIO and JQFIO output text such as json responses into a file

QFIO is just for normal stuff like the header of a request while JQFIO is specifically for JSON
it parses it in nlohmann json first then pretty prints it to a file, but if it fails it just outputs normally to a file without pretty print

*/
void QFIO(std::string filename, std::string data);
void JQFIO(std::string filename, std::string json);

// State holds the results of a request
typedef struct _State
{
	int http_state;
	std::string message;
}State;

// Item holds items from user's saved
typedef struct _Item
{
	bool is_self;
	bool is_comment;
	std::string permalink;
	std::string fullname, id; // I have both fullname and id because I want to see if resaving/unsaving it will take either fullname or id
	std::string link_url; // The url in a linked post
	std::string domain; // I have this here so I can know if the linked url leads to a subreddit and if it uses new Reddit or old Reddit
}Item;



size_t writedat(char* buffer, size_t size, size_t nmemb, std::string& src);

class Saver
{
public:

	Saver(std::string username, std::string password, std::string client_id, std::string secret, std::string useragent);
	/* these toggles enable using old Reddit or printing out the linked urls with the saved urls
	   by default old Reddit and outputting linked urls are disabled */
	void OldRedditToggle(bool val) { this->use_old_reddit = val; }
	void ListLinkedUrls(bool val) { this->list_linked_urls = val; }
	State Save(std::string fullname);
	State UnSave(std::string fullname);
	// AccessSaved is the function to retrieve saved items, and they're stored in std::vector<Item*>& saved
	State AccessSaved(std::vector<Item*>& saved);
	State AccessReddit() { return this->obtain_token(false); }
	State RefreshToken() { return this->obtain_token(true); }
	/*
		These function output your saved in a list or in JSON format
		BackupAsJson is specifically used to backup saved incase anything goes wrong,
		and OutputSaved lists saved urls in a text file for use in wget or HTTrack
	*/
	bool BackupAsJson(std::string filename, std::vector<Item*>& src);
	bool OutputSaved(std::string filename, std::vector<Item*>& src);

private:
	std::string username;
	std::string password;
	std::string client_id;
	std::string secret;
	std::string useragent;
	// After stores the id for the next page of saved content
	std::string after;

	/*
		All the request_done and chrono related variables
		keep track of how many requests are being done.
		This is in place because Reddit has a limit of 60 requests per minute
		and 600 requests per token/session per hour
	*/
	int requests_done;
	int request_done_in_current_minute;

	// now and then keep track of session requests
	std::chrono::system_clock::time_point now;
	std::chrono::system_clock::time_point then;

	// mnow and mthen keep track of how many requests per minute
	std::chrono::system_clock::time_point mnow;
	std::chrono::system_clock::time_point mthen;
	/*

		The functions below are related to the variables above.
		
		restart_minute_clock restarts mnow and mthen when 60 seconds has elapsed 
		or when 60 requests have been reached in the current minute

		is_time_up just checks if 600 requests have occured in the hour
		or if an hour has elapsed. Returns true if the previous conditions have been met.

		is_mtime_up is different from is_time_up, instead of returning a boolean if
		60 requests/seconds have passed It will stall until 60 seconds have passed then it resets
	*/
	void restart_minute_clock();
	void is_mtime_up();
	bool is_time_up() { if (now >= then) { return true; } return false; }

	bool use_old_reddit;
	bool list_linked_urls;

	std::string token; // where our access token goes
	/*
	
		obtain_token gets the access token from Reddit and refreshes
		for a new one when refresh is true

		SaveToggle saves or unsaved depending if remove is enabled or not
	
	*/
	State obtain_token(bool refresh);
	State SaveToggle(std::string fullname, bool remove);
	// Just gets the user's save items
	State get_saved_items(std::vector<Item*>& sitem);


};