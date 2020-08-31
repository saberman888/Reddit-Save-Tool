
#pragma once

#include "curl/curl.h"
#include <string>
#include <iosfwd>
#include <cstdlib>
#include <cassert>




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
	BasicRequest() : Handle(nullptr), headers(nullptr) {}
	/*
	Setup(std::string, bool), assigns a URL to the cURL handle and adjusts it to be
	either a GET request or a POST request
	*/
	void Setup(std::string URL, bool POST = false);
	void SetHeaders(std::string header);
	template<typename Y>
	void SetOpt(CURLoption option, Y data);
	template<typename Y>
	void GetInfo(CURLINFO option, Y* data);
	void SetPostfields(std::string params);
	void SetCreds(std::string usrpwd);
	void SetUserAgent(std::string useragent);
	void SendRequest();
	void Cleanup();

	struct State
	{
		State() : HttpState(0l), Message(), buffer(), ContentType(), result(CURLE_OK){}
		long HttpState;
		std::string Message;
		std::string buffer;
		std::string ContentType;
		CURLcode result;
		bool AllGood() { return (Message.empty() && HttpState == 200 && result == CURLE_OK); }
	}Response;
private:
	CURL* Handle;
	struct curl_slist *headers;
	void WriteToState();
	void Perform();
};

size_t writedat(char* buffer, size_t size, size_t nmemb, std::string& dest);
