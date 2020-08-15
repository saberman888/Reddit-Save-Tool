#pragma once
#include <string>
#include "base.hpp"
#include <fstream>
#include "BasicRequest.hpp"
#include "base.hpp"
#include "nlohmann/json.hpp"
#include "curl/curl.h"


class RedditAccess {
public:
	RedditAccess();
	// obtain_token gets the access token from Reddit*/
	State AccessReddit() { return this->obtain_token(); }
	State RedditGetRequest(std::string endpoint, std::string& buffer);
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
