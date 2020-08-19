#pragma once

#include "curl/curl.h"
#include <string>
#include <iosfwd>
#include <cstdlib>
#include <cassert>

typedef struct State
{
	long http_state;
	std::string message;
}State;


typedef struct _BasicRequestRAII
{
	_BasicRequestRAII();
	~_BasicRequestRAII();
}BasicRequestRAII;

/*
	Pretty basic cURL wrapper made to do once request at a time, which is ideal for this project
*/

class BasicRequest
{
public:
	BasicRequest() : headers(nullptr), Handle(nullptr) {}
	void Setup(std::string URL, bool POST = false);
	void SetHeaders(std::string header);
	template<typename Y>
	void SetOpt(CURLoption option, Y data);
	template<typename Y>
	void GetInfo(CURINFO option, Y data);
	void WriteTo(std::string& buffer);
	void AddParams(std::string params);
	void AddUserPWD(std::string usrpwd);
	void AddUserAgent(std::string useragent);
	State SendRequest();
	void Cleanup();

private:
	CURL* Handle;
	CURLcode result;
	struct curl_slist *headers;
	State Perform();
	size_t writedat(char* buffer, size_t size, size_t nmemb, std::string& src);
};