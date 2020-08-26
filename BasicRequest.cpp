#include "BasicRequest.hpp"
#include <iostream>


void BasicRequest::Setup(std::string URL, bool POST)
{
	Handle = curl_easy_init();
	if (!Handle)
	{
		std::cerr << "Error, Failed to allocate cURL Handle" << std::endl;
		abort();
	}
	SetOpt(CURLOPT_URL, URL.c_str());
	if (POST)
		SetOpt(CURLOPT_POST, 1L);
}

State BasicRequest::SendRequest()
{
	return Perform();
}

void BasicRequest::Cleanup()
{
	// Free Handle and headers once done with the operation
	curl_easy_cleanup(Handle);
	curl_slist_free_all(headers);

	// re-assign to nullptr
	headers = nullptr;
	Handle = nullptr;
}

void BasicRequest::SetHeaders(std::string header)
{
	assert(Handle != nullptr);
	headers = curl_slist_append(headers, header.c_str());
}

template<typename Y>
void BasicRequest::SetOpt(CURLoption option, Y data)
{
	assert(Handle != nullptr);
	result = curl_easy_setopt(this->Handle, option, data);
	if (result != CURLE_OK)
	{
		std::cerr << curl_easy_strerror(result) << std::endl;
		abort();
	}
}

template<typename Y>
void BasicRequest::GetInfo(CURLINFO option, Y data)
{
	assert(Handle != nullptr);
	result = curl_easy_getinfo(this->Handle, option, data);
	if (result != CURLE_OK)
	{
		std::cerr << curl_easy_strerror(result) << std::endl;
		abort();
	}
}

void BasicRequest::WriteTo(std::string& buffer)
{
	SetOpt(CURLOPT_WRITEFUNCTION, &BasicRequest::writedat);
	SetOpt(CURLOPT_WRITEDATA, buffer);
}

void BasicRequest::AddParams(std::string params)
{
	SetOpt(CURLOPT_POSTFIELDS, params.c_str());
}

void BasicRequest::AddUserPWD(std::string usrpwd)
{
	SetOpt(CURLOPT_USERPWD, usrpwd.c_str());
}

void BasicRequest::AddUserAgent(std::string useragent)
{
	SetOpt(CURLOPT_USERAGENT, useragent.c_str());
}


State BasicRequest::Perform()
{
	assert(Handle != nullptr);
	State ReturnResult;
	if (headers != nullptr)
		SetOpt(CURLOPT_HTTPHEADER, headers);
	this->result = curl_easy_perform(Handle);

	curl_easy_getinfo(Handle, CURLINFO_RESPONSE_CODE, &ReturnResult.http_state);
	if (result != CURLE_OK) {
		ReturnResult.message = curl_easy_strerror(result);
		std::cerr << curl_easy_strerror(result) << std::endl;
	}
	return ReturnResult;
}

size_t BasicRequest::writedat(char* buffer, size_t size, size_t nmemb, std::string& src)
{
	for (size_t i = 0; i < size * nmemb; i++)
	{
		src.push_back(buffer[i]);
	}
	return size * nmemb;
}

_BasicRequestRAII::_BasicRequestRAII()
{
	CURLcode Result = curl_global_init(CURL_GLOBAL_ALL);
	if (Result != CURLE_OK)
	{
		std::cerr << "Error, failed to initalize cURL" << std::endl;
		std::abort();
	}
}


_BasicRequestRAII::~_BasicRequestRAII()
{
	curl_global_cleanup();
}
