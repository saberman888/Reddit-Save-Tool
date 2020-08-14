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
};
