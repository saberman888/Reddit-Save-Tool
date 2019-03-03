#pragma once

#include "curl/curl.h"
#include <vector>
#include <string>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iosfwd>

#define POST_LIMIT 1000
const std::string initial_url = "https://www.reddit.com/api/v1/access_token";
#define SCOPE "%20save%20account%20read"
void QFIO(std::string filename, std::string data);
struct State
{
	int http_state;
	std::string message;
};

size_t writedat(char* buffer, size_t size, size_t nmemb, std::string& src);


class Saver
{
public:

	void setAccessData(std::string username, std::string password, std::string client_id, std::string secret, std::string useragent);
	State Save(std::string fullname) { return this->SaveToggle(fullname, false);  }
	State UnSave(std::string fullname) { return this->SaveToggle(fullname, true); }
	State AccessReddit() { return this->obtain_token(false); }
	State RefreshToken() { return this->obtain_token(true); }

private:
	std::string username;
	std::string password;
	std::string client_id;
	std::string secret;
	std::string useragent;

	std::string token;

	State obtain_token(bool refresh);
	State SaveToggle(std::string fullname, bool remove);
};