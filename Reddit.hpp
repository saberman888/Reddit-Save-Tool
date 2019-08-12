#pragma once
#include <string>
#include "base.hpp"
#include <chrono>
#if (defined(__GNUC__) || defined(_MSC_VER)) && !(defined(__MINGW64__) || defined(__MINGW32__))
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif
#include <ctime>
#include "nlohmann/json.hpp"
#include <fstream>
#include "curl/curl.h"
#include <memory>

class RedditAccess {
public:
	RedditAccess(CMDArgs* arg);
	~RedditAccess();
	void init_logs();
	bool load_login_info();
	/*obtain_token gets the access token from Redditand refreshes
		for a new one when refresh is true*/
	State AccessReddit() { return this->obtain_token(false); }
	State RefreshToken() { return this->obtain_token(true); }
	std::vector<struct creds*> accounts;
	struct creds* Account;
	// After stores the id for the next page of saved content
	std::string after;

	State obtain_token(bool refresh);
	std::string token; // where our access token goes
	std::fstream* log;
	std::string logpath, mediapath;
	std::streambuf* old_rdbuf;
	CMDArgs* args;
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
	void tick();



};