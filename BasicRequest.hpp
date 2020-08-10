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
	void SetHeaders(std::string header);
	template<typename Y>
	void SetAttribute(CURLoption option, Y data);
	State SendRequest(std::string URL, bool POST=false);
	void Cleanup();

private:
	CURL* Handle;
	CURLcode result;
	struct curl_slist *headers;
	State Perform();

};
