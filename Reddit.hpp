#pragma once
#include <string>
#include "base.hpp"
#include <fstream>
#include "BasicRequest.hpp"
#include "base.hpp"
#include "nlohmann/json.hpp"
#include "curl/curl.h"

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
constexpr bool IsUnixBased = true;
#else
constexpr bool IsUnixBased = false;
#endif

class RedditAccess {
public:
	RedditAccess();
	// obtain_token gets the access token from Reddit*/
	State AccessReddit() { return this->obtain_token(); }
	State RedditGetRequest(std::string endpoint, std::string& buffer);
	bool IsLoggedIn;
	fs::path MediaPath;
	BasicRequest RedditHandle;

	// Where we're going to store user information when we parse it from the config file
	struct
	{
		std::string Token;
		std::string Username, Password;
		std::string ClientId, Secret, UserAgent;
	} UserAccount;

private:
	State obtain_token();
	std::string ReadJson(std::string json);
};
