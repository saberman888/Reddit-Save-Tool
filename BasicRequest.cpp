#include "BasicRequest.hpp"
#include <iostream>

static size_t writedat(char* buffer, size_t size, size_t nmemb, std::string& dest)
{
    for(size_t i = 0; i < size * nmemb; i++)
        dest.push_back(buffer[i]);
    return size * nmemb;
}

void BasicRequest::Setup(std::string URL, bool POST)
{
	Handle = curl_easy_init();
	if (!Handle)
	{
		throw std::runtime_error("Error, Failed to allocate cURL Handle");
	}
	this->URL = URL;
	// Initialize Response's variable to be empty incase if there is anything already there
	this->Response = new State();
	if (POST)
		SetOpt(CURLOPT_POST, 1L);
	WriteToState();

#ifdef CURLVERBOSE
	SetOpt(CURLOPT_VERBOSE, 1L);
#endif

}

void BasicRequest::Cleanup()
{
	// Free Handle and headers once done with the operation
	curl_easy_cleanup(Handle);
	curl_slist_free_all(headers);

	// re-assign to nullptr
	headers = nullptr;
	Handle = nullptr;

	// Clean up Response
	delete Response;
	Response = nullptr;
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
	auto result = curl_easy_setopt(this->Handle, option, data);
	if (Response->result != CURLE_OK)
	{
		throw std::runtime_error(curl_easy_strerror(result));
	}
}

template<typename Y>
void BasicRequest::GetInfo(CURLINFO option, Y* data)
{
	assert(Handle != nullptr);
	auto result = curl_easy_getinfo(this->Handle, option, data);
	if (Response->result != CURLE_OK)
	{
		throw std::runtime_error(curl_easy_strerror(result));
	}
}

void BasicRequest::WriteToState()
{
	SetOpt(CURLOPT_WRITEFUNCTION, &writedat);
	SetOpt(CURLOPT_WRITEDATA, &Response->buffer);
}

void BasicRequest::SetPostfields(std::string params)
{
	SetOpt(CURLOPT_COPYPOSTFIELDS, params.c_str());
}

void BasicRequest::SetURLParameters(const std::map<std::string, std::string>& parameters )
{
	for (std::map<std::string, std::string>::const_iterator it = parameters.begin(); it != parameters.end(); it++)
	{
		URL.append(
			((it == parameters.begin())? "?" : "&")
			+ it->first
			+ "="
			+ Escape(it->second)
			);
	}
}

void BasicRequest::SetCreds(std::string usrpwd)
{
	SetOpt(CURLOPT_USERPWD, usrpwd.c_str());
}

void BasicRequest::SetUserAgent(std::string useragent)
{
	SetOpt(CURLOPT_USERAGENT, useragent.c_str());
}


std::string BasicRequest::Escape(const std::string data)
{
	// Check if data has special characters, if not just return itself. 
	// No need to escape it if doesn't have any special characters
	if (data.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_") != std::string::npos)
	{
		return data;
	}
	else {
		// Escape data with cURL and check if it didn't cause error
		auto escaped = curl_easy_escape(Handle, data.c_str(), static_cast<int>(data.size()));
		if (!escaped) {
			return data;
		}
		// Copy the escaped c-string into a std::string and free escaped
		// because escaped is returned as manually allocated and we don't want to cause a memory leak
		std::string escapedString = std::string(escaped);
		curl_free(escaped);

		return escapedString;
	}
}


State BasicRequest::SendRequest()
{
	assert(Handle != nullptr);
	if (headers != nullptr)
		SetOpt(CURLOPT_HTTPHEADER, headers);
	std::cout << URL << std::endl;
	SetOpt(CURLOPT_URL, URL.c_str());
	Response->result = curl_easy_perform(Handle);

	if (Response->result != CURLE_OK) {
		Response->Message = curl_easy_strerror(Response->result);

	}

	if (Response->result != CURLE_OPERATION_TIMEDOUT)
	{
		GetInfo(CURLINFO_RESPONSE_CODE, &Response->HttpState);
		char* tempContentType = nullptr;
		GetInfo(CURLINFO_CONTENT_TYPE, &tempContentType);

		if (tempContentType)
			Response->ContentType = tempContentType;
	}
	
	return *Response;
}

time_t BasicRequest::GetUTCTime(const std::string data)
{
	auto time = curl_getdate(data.c_str(), nullptr);
	if(!time)
	{
        throw std::runtime_error("Error: Got NULL from curl_getdate instead of time_t. Possible invalid date string format");
	} else {
		return time;
	}
}

time_t BasicRequest::GetCurrentUTCTime()
{
    time_t data;
    time(&data);
    return data;
}

std::string BasicRequest::UTCToString(time_t time, std::string format)
{
#if defined(_MSC_VER)
    struct tm timeinfo;
#else
    struct tm* timeinfo;
#endif

    char buffer[80];
#if defined(_MSC_VER)
	localtime_s(&timeinfo, &time);
    strftime(buffer, 80, format.c_str(), &timeinfo);
#else
    timeinfo = std::gmtime(&time);
    std::strftime(buffer, 80, format.c_str(), timeinfo);
#endif

    return std::string(buffer);
}




_BasicRequestRAII::_BasicRequestRAII()
{
	CURLcode Result = curl_global_init(CURL_GLOBAL_ALL);
	if (Result != CURLE_OK)
	{
		throw std::runtime_error("Failed to initialize curl global init");
	}
}


_BasicRequestRAII::~_BasicRequestRAII()
{
	curl_global_cleanup();
}
