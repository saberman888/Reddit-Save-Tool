
#pragma once

#include "curl/curl.h"
#include <string>
#include <iosfwd>
#include <cstdlib>
#include <cassert>
#include <map>
#include <exception>
#include <ctime>

#define NDEBUG 1

class State
{
public:
	State() : HttpState(0l), Message(), buffer(), ContentType(), result(CURLE_OK) {}
	long HttpState;
	std::string Message;
	std::string buffer;
	std::string ContentType;
	CURLcode result;
	bool AllGood() { return (Message.empty() && HttpState == 200 && result == CURLE_OK); }
};

class cURLError : std::exception
{
public:
	cURLError(std::string message) : message(message){}
	std::string what() { return message; }
private:
	std::string message;
};

/*
	An RAII struct that manages curl_global_init and curl_global_cleanup
*/
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
	BasicRequest() : Handle(nullptr), URL(), Response(nullptr), headers(nullptr)  {}
	/*
		Sets up the operation by initializing the cURL, initializes other internal variables like Response and 
		assigns it to be written to. You can also set it up as a POST request by passing true into the last parameter 
		and make cURL output more verbosely by passing NDEBUG=Off in CMake.

		Throws cURLError if CURL* Handle fails to initialize.
	*/
	void Setup(std::string URL, bool POST = false);
	/*
		Adds headers to the cURL handle. It's equivalent to using curl_slist_append.
	*/
	void SetHeaders(std::string header);
	/*
		Wrapper for curl_easy_setopt. Throws cURLError if anything goes wrong
	*/
	template<typename Y>
	void SetOpt(CURLoption option, Y data);
	/*
		Wrapper for curl_easy_getinfo. Throws cURLError if anything goes wrong
	*/
	template<typename Y>
	void GetInfo(CURLINFO option, Y* data);
	void SetPostfields(std::string params);
	void SetURLParameters(const std::map<std::string, std::string>& parameters);
	void SetCreds(std::string usrpwd);
	void SetUserAgent(std::string useragent);

	/*
		Escapes any special chacters in a string. If it doesn't have any special characeters, it returns itself
		else it returns a string with special characters URL encoded
	*/
	std::string Escape(const std::string data);
	State SendRequest();
	void Cleanup();

	static time_t GetUTCTime(const std::string data);
    static time_t GetCurrentUTCTime();
    static std::string UTCToString(time_t time, std::string format);
private:
	CURL* Handle;
	std::string URL;
	State* Response;
	struct curl_slist *headers;
	void WriteToState();
};
