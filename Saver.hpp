#pragma once

#include "curl/curl.h"
#include <vector>
#include <string>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iosfwd>
#include <sstream>
#include <map>
#include <boost/algorithm/string.hpp>
#include <stdlib.h>
#include <chrono>
#include <ratio>

#define POST_LIMIT 1000
#define LIMIT_PER_TOKEN 600
#define RQ_PER_MINUTE 60

const std::string initial_url = "https://www.reddit.com/api/v1/access_token";
#define SCOPE "%20save%20account%20read"
void QFIO(std::string filename, std::string data);
struct State
{
	int http_state;
	std::string message;
};

size_t writedat(char* buffer, size_t size, size_t nmemb, std::string& src);
std::map<std::string, std::string> MapHeaders(std::string source);

class Saver
{
public:

	void setAccessData(std::string username, std::string password, std::string client_id, std::string secret, std::string useragent);
	State Save(std::string fullname);
	State UnSave(std::string fullname);
	State AccessSaved();
	State AccessReddit() { return this->obtain_token(false); }
	State RefreshToken() { return this->obtain_token(true); }

private:
	std::string username;
	std::string password;
	std::string client_id;
	std::string secret;
	std::string useragent;

	int requests_done;

	std::chrono::system_clock::time_point now;
	std::chrono::system_clock::time_point then;

	std::string token;

	State obtain_token(bool refresh);
	State SaveToggle(std::string fullname, bool remove);
	bool is_time_up() {if (now >= then) { return true; } return false;}
};