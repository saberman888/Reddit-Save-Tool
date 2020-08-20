#include "Reddit.hpp"
#include <iostream>

RedditAccess::RedditAccess()
{
}


State RedditAccess::RedditGetRequest(std::string endpoint, std::string& buffer)
{
	State result;
	std::string URL =
		"https://oauth.reddit.com/"
		+ endpoint;
	RedditHandle.Setup(URL);
	RedditHandle.AddUserAgent(UserAccount.UserAgent);
	RedditHandle.SetOpt(CURLOPT_SSL_VERIFYPEER, 0L);
	RedditHandle.WriteTo(buffer);
	RedditHandle.SetHeaders("Authorization: bearer " + UserAccount.Token);
	result = RedditHandle.SendRequest();
	return result;
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


State RedditAccess::obtain_token()
{
	std::string json;
	State result;
	RedditHandle.Setup("https://www.reddit.com/api/v1/access_token", true);
	RedditHandle.SetOpt(CURLOPT_SSL_VERIFYPEER, 1L);
	RedditHandle.WriteTo(json);
	RedditHandle.AddUserAgent(UserAccount.UserAgent);
	RedditHandle.AddUserPWD(UserAccount.ClientId + ":" + UserAccount.Secret);
	std::string postfields =
		"grant_type=password&username"
		+ UserAccount.Username
		+ "&password="
		+ UserAccount.Password
		+ "&scope=%20save%20read%20history";
	RedditHandle.AddParams(postfields);
	result = RedditHandle.SendRequest();
	result.message = ReadJson(json);
	RedditHandle.Cleanup();
	return result;
}
