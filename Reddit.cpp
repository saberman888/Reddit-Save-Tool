#include "Reddit.hpp"
#include <iostream>

bool RedditAccess::AccessReddit()
{
	// Connect to Reddit and authenticate with them
	State result = AccessToken();
	// Try to parse whatever response we get
	ReadJson(result);
	if (!result.AllGood())
	{
		std::cerr << "Error, failed to get Reddit access token" << std::endl;
		std::cerr << "Error " << result.HttpState << ": " << result.Message << std::endl;
		return false;
	}
	return true;
}

State RedditAccess::RedditGetRequest(std::string endpoint)
{
	BasicRequest handle;
	std::string URL =
		"https://oauth.reddit.com/"
		+ endpoint;
	handle.Setup(URL);
	handle.SetUserAgent(UserAccount.UserAgent);
	handle.SetHeaders("Authorization: bearer " + UserAccount.Token);
	State result = handle.SendRequest();
	handle.Cleanup();
	return result;
}

void RedditAccess::ReadJson(State& Response)
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


State RedditAccess::AccessToken()
{
	BasicRequest handle;
	handle.Setup("https://www.reddit.com/api/v1/access_token", true);
	handle.SetUserAgent(UserAccount.UserAgent);
	std::string usrpwd = UserAccount.ClientId + ":" + UserAccount.Secret;
	handle.SetCreds(usrpwd);
	std::string postfields = 
		"grant_type=password&username="
		+ UserAccount.Username
		+ "&password="
		+ UserAccount.Password
		+ "&scope=%20save%20read%20history";
	handle.SetPostfields(postfields);
	State result = handle.SendRequest();
	handle.Cleanup();
	return result;
}
