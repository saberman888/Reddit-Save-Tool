#include "Reddit.hpp"
#include <iostream>

bool RedditAccess::AccessReddit()
{
	// Connect to Reddit and authenticate with them
	AccessToken();
	// Try to parse whatever response we get
	ReadJson();
	if (!Response.AllGood())
	{
		std::cerr << "Error, failed to get Reddit access token" << std::endl;
		std::cerr << "Error " << Response.HttpState << ": " << Response.Message << std::endl;
		return false;
	}
	return true;
}

void RedditAccess::RedditGetRequest(std::string endpoint)
{
	std::string URL =
		"https://oauth.reddit.com/"
		+ endpoint;
	Setup(URL);
	SetUserAgent(UserAccount.UserAgent);
	SetHeaders("Authorization: bearer " + UserAccount.Token);
	SendRequest();
	Cleanup();
}

void RedditAccess::ReadJson()
{

	try {
		nlohmann::json parse = nlohmann::json::parse(Response.buffer);
		if (parse.contains("access_token"))
		{
			UserAccount.Token = parse.at("access_token").get<std::string>();
		}
		else if (parse.contains("error")) {
			if (parse.at("error").is_number_integer())
			{
				Response.Message = parse.at("message").get<std::string>();
			}
			else {
				Response.Message = parse.at("error").get<std::string>();
			}
		}
		else {
			Response.Message = "Error, Unknown error";
		}
	}
	catch (nlohmann::json::exception& e) {
		std::cerr << "An error occured, " << e.what() << std::endl;
	}
}


void RedditAccess::AccessToken()
{
	Setup("https://www.reddit.com/api/v1/access_token", true);
	SetUserAgent(UserAccount.UserAgent);
	std::string usrpwd = UserAccount.ClientId + ":" + UserAccount.Secret;
	SetCreds(usrpwd);
	std::string postfields = 
		"grant_type=password&username="
		+ UserAccount.Username
		+ "&password="
		+ UserAccount.Password
		+ "&scope=%20save%20read%20history";
	SetPostfields(postfields);
	SendRequest();
	Cleanup();
}
