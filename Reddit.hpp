#pragma once
#include <string>
#include <fstream>
#include "BasicRequest.hpp"
#include "base.hpp"
#include "nlohmann/json.hpp"
#include "curl/curl.h"
#include "Imgur.hpp"


class RedditAccess {
public:
	RedditAccess(){};
	// obtain_token gets the access token from Reddit*/
	void AccessToken();
	void RedditGetRequest(std::string endpoint);
	BasicRequest RedditHandle;
	State response;
	// Where we're going to store user information when we parse it from the config file
	struct
	{
		std::string Token;
		std::string Username, Password;
		std::string ClientId, Secret, UserAgent;
	} UserAccount;

private:
	std::string ReadJson(std::string json);
};
