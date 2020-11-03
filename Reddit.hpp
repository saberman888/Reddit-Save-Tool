#pragma once
#include <string>
#include <fstream>
#include "BasicRequest.hpp"
#include "nlohmann/json.hpp"
#include "curl/curl.h"
#include "Imgur.hpp"


class RedditAccess{
public:
	RedditAccess(){};
	// obtain_token gets the access token from Reddit
	bool AccessReddit();
	State RedditGetRequest(std::string endpoint);
	// Where we're going to store user information when we parse it from the config file
	struct
	{
		std::string Token;
		std::string Username, Password;
		std::string ClientId, Secret, UserAgent;
	} UserAccount;

private:
	State AccessToken();
	void ReadJson(State& Response);
};
