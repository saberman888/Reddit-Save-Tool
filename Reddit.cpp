#include "Reddit.hpp"
#include <iostream>


void RedditAccess::RedditGetRequest(std::string endpoint)
{
	std::string URL =
		"https://oauth.reddit.com/"
		+ endpoint;
	RedditHandle.Setup(URL, &response);
	RedditHandle.AddUserAgent(UserAccount.UserAgent);
	RedditHandle.SetOpt(CURLOPT_SSL_VERIFYPEER, 0L);
	RedditHandle.SetHeaders("Authorization: bearer " + UserAccount.Token);
	RedditHandle.SendRequest();
}

std::string RedditAccess::ReadJson(std::string json)
{

	try {
		nlohmann::json parse = nlohmann::json::parse(json);
		if (parse.contains("access_token"))
		{
			UserAccount.Token = parse.at("access_token").get<std::string>();
		}
		else if (parse.contains("message")) {
			return parse.at("message").get<std::string>();
		}
		if (parse.contains("invalid_grant")) {
			return "Invalid login credentials";
		}
		else {
			return "Unknown error";
		}
	}
	catch (nlohmann::json::exception& e) {
		std::cerr << "An error occured, " << e.what() << std::endl;
		throw;
	}
}


void RedditAccess::AccessToken()
{
	RedditHandle.Setup("https://www.reddit.com/api/v1/access_token", &response, true);
	RedditHandle.SetOpt(CURLOPT_SSL_VERIFYPEER, 1L);
	RedditHandle.AddUserAgent(UserAccount.UserAgent);
	RedditHandle.AddUserPWD(UserAccount.ClientId + ":" + UserAccount.Secret);
	std::string postfields =
		"grant_type=password&username"
		+ UserAccount.Username
		+ "&password="
		+ UserAccount.Password
		+ "&scope=%20save%20read%20history";
	RedditHandle.AddParams(postfields);
	RedditHandle.SendRequest();
	RedditHandle.Cleanup();
	response.Message = ReadJson(response.buffer);
}
