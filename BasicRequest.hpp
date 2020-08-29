
#pragma once

#include "curl/curl.h"
#include <string>
#include <iosfwd>
#include <cstdlib>
#include <cassert>


class State
{
public:
	State() : HttpState(0l), Message(), buffer(){}
	long HttpState;
	std::string Message;
	std::string buffer;
};


typedef struct _BasicRequestRAII
{
	_BasicRequestRAII();
	~_BasicRequestRAII();
}BasicRequestRAII;

/*
	Pretty basic cURL wrapper made to do one request at a time, which is ideal for this project
*/

class BasicRequest
{
public:
	BasicRequest() : Handle(nullptr), result(CURLE_OK), headers(nullptr), dest(nullptr)  {}
	/*
	Setup(std::string, bool), assigns a URL to the cURL handle and adjusts it to be
	either a GET request or a POST request
	*/
	void Setup(std::string URL, State* dest, bool POST = false);
	void SetHeaders(std::string header);
	template<typename Y>
	void SetOpt(CURLoption option, Y data);
	template<typename Y>
	void GetInfo(CURLINFO option, Y* data);
	void AddParams(std::string params);
	void AddUserPWD(std::string usrpwd);
	void AddUserAgent(std::string useragent);
	void SendRequest();
	void Cleanup();

private:
	CURL* Handle;
	CURLcode result;
	struct curl_slist *headers;
	State* dest;
	void WriteToState();
	void Perform();
	size_t writedat(char* buffer, size_t size, size_t nmemb, std::string& dest);
};
