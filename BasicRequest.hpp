#pragma once

#include "curl/curl.h"
#include <string>
#include <iosfwd>
#include <cstdlib>
#include <cassert>
#include <chrono>
#include <utility>

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
	BasicRequest(std::pair<int, int> limits) :limits(limits), RefreshFunction(nullptr), 
			headers(nullptr), Handle(nullptr) {}
	BasicRequest(State(*RefreshFunction)(bool), std::pair<int, int> limits) : RefreshFunction(RefreshFunction), limits(limits), 
		headers(nullptr), Handle(nullptr) {}
	void SetHeaders(std::string header);
	template<typename Y>
	void SetAttribute(CURLoption option, Y data);
	State SendRequest(std::string URL, bool POST=false);
	void Cleanup();

private:
	CURL* Handle;
	CURLcode result;
	struct curl_slist *headers;
	State(*RefreshFunction)(bool);
	State Perform();

	/*
		All the request_done and chrono related variables
		keep track of how many requests are being done.
		This is in place because Reddit has a limit of 60 requests per minute
		and 600 requests per token/session per hour
	*/
	std::pair<int, int> limits;

	// now and then keep track of session requests
	std::chrono::system_clock::time_point now;
	std::chrono::system_clock::time_point then;

	// mnow and mthen keep track of how many requests per minute
	std::chrono::system_clock::time_point mnow;
	std::chrono::system_clock::time_point mthen;

	/*

		The functions below are related to the variables above.

		restart_minute_clock restarts mnow and mthen when 60 seconds has elapsed
		or when 60 requests have been reached in the current minute

		is_time_up just checks if 600 requests have occured in the hour
		or if an hour has elapsed. Returns true if the previous conditions have been met.

		is_mtime_up is different from is_time_up, instead of returning a boolean if
		60 requests/seconds have passed It will stall until 60 seconds have passed then it resets
	*/
	void restart_minute_clock();
	void is_mtime_up();
	bool is_time_up() { if (now >= then) { return true; } return false; }
	void tick();


};