#pragma once
#include <string>
#include "base.hpp"
#include <chrono>
#include <ctime>
#include <fstream>
#include "BasicRequest.hpp"
#include "nlohmann/json.hpp"
#include "curl/curl.h"

constexpr int POST_LIMIT = 1000;
constexpr int LIMIT_PER_TOKEN = 600;
constexpr int RQ_PER_MINUTE = 60;

#if defined(__cpp_lib_filesystem)
	#include <filesystem>
	namespace fs = std::filesystem;
#elif defined(__cpp_lib_experimental_filesystem)
	#include <experimental/filesystem>
	namespace fs = std::experimental::filesystem;
#elif defined(USE_EXP_FS)
	#include <experimental/filesystem>
	namespace fs = std::experimental::filesystem;
#else
	#error "No filesystem support found :("
#endif


#if defined(unix) || defined(_unix)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
constexpr bool IsUnixBased = true;
#else
constexpr bool IsUnixBased = false;
#include <Synchapi.h>
#endif

class RedditAccess {
public:
	RedditAccess();
	bool LoadLogins();
	/*obtain_token gets the access token from Redditand refreshes
		for a new one when refresh is true*/
	State AccessReddit() { return this->obtain_token(false); }
	State RefreshToken() { return this->obtain_token(true); }

	bool IsLoggedIn;
	CMDArgs args;
	fs::path StoragePath;

private:
	// Where we're going to store user information when we parse it from the config file
	struct
	{
		std::string Username, Password;
		std::string ClientId, Secret, UserAgent;
	} UserAccount;

	struct
	{
		int TotalPosts;
		std::string after;
		std::vector<Item> posts;
	};

	State obtain_token(bool refresh);
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


	/*
		Below here is an implementation for better support for imgur images
	*/
	// Imgur credentials
	std::string imgur_client_id;
	bool imgur_enabled;
	State retrieve_album_images(std::string album_id, std::vector<std::string>& URLs);
	State retrieve_imgur_image(std::string imghash, std::string& URL);
	State download_item(const char* URL, std::string& buf);
};
