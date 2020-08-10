#include "BasicRequest.hpp"
#include <iostream>


State BasicRequest::SendRequest(std::string URL, bool POST)
{
	Handle = curl_easy_init();
	if (!Handle)
	{
		std::cerr << "Error, Failed to allocate cURL Handle" << std::endl;
		abort();
	}
	SetAttribute(CURLOPT_URL, URL.c_str());
	if (POST)
		SetAttribute(CURLOPT_POST, 1L);
	return Perform();
}

void BasicRequest::Cleanup()
{
	// Free Handle and headers once done with the operation
	curl_easy_cleanup(Handle);
	curl_slist_free_all(headers);

	// re-assign to nullptr
	headers = nullptr;
}

void BasicRequest::SetHeaders(std::string header)
{
	assert(!Handle);
	curl_slist_append(headers, header.c_str());
}

template<typename Y>
void BasicRequest::SetAttribute(CURLoption option, Y data)
{
	assert(!Handle);
	result = curl_easy_setopt(Handle, option, data);
	if (result != CURLE_OK)
	{
		std::cerr << curl_easy_strerror(result) << std::endl;
		abort();
	}
}


State BasicRequest::Perform()
{
	assert(!Handle);
	State ReturnResult;
	result = curl_easy_perform(Handle);

	curl_easy_getinfo(Handle, CURLINFO_RESPONSE_CODE, &ReturnResult.http_state);
	if (result != CURLE_OK) {
		ReturnResult.message = curl_easy_strerror(result);
		std::cerr << curl_easy_strerror(result) << std::endl;
	}
	return ReturnResult;
}

_BasicRequestRAII::_BasicRequestRAII()
{
	CURLcode Result = curl_global_init(CURL_GLOBAL_ALL);
	if (Result != CURLE_OK)
	{
		std::cerr << "Error, failed to initalize cURL" << std::endl;
		abort();
	}
}

_BasicRequestRAII::~_BasicRequestRAII()
{
	curl_global_cleanup();
}


void BasicRequest::restart_minute_clock()
{
	std::chrono::minutes one_minute(1);
	mnow = std::chrono::system_clock::now();
	mthen = mnow + one_minute;
	request_done_in_current_minute = 0;
}

void BasicRequest::is_mtime_up()
{
	if ((request_done_in_current_minute == RQ_PER_MINUTE)) {
#ifdef _DEBUG
		std::cout << "60 requests limit per minute has hit, stalling." << std::endl;
#endif
		// Stall then reset time
#ifdef _WIN32
		Sleep(60000);
#elif _unix
		usleep(60);
#else
		usleep(60);
#endif
		restart_minute_clock();
	}
	else if (mnow >= mthen) {
		restart_minute_clock();
	}
}

void BasicRequest::tick()
{
	requests_done += 1;
	if (now < then) {
		this->request_done_in_current_minute += 1;
	}
}