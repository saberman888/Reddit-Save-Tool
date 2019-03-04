#include "Saver.hpp"
#include <iostream>

void QFIO(std::string filename, std::string data)
{
	std::ofstream(filename.c_str(), std::ios::out) << data; 
}

size_t writedat(char* buffer, size_t size, size_t nmemb, std::string& src)
{
	for (size_t i = 0; i < size * nmemb; i++)
	{
		src.push_back(buffer[i]);
	}
	return size * nmemb;
}


void Saver::setAccessData(std::string username, std::string password, std::string client_id, std::string secret, std::string useragent)
{
	this->username = username;
	this->password = password;
	this->client_id = client_id;
	this->secret = secret;
	this->useragent = useragent;
}

State Saver::SaveToggle(std::string fullname, bool remove)
{
	CURL* handle;
	CURLcode result;
	std::string json;
	int responsecode = 0;
	State response;
	std::string rheader;

	handle = curl_easy_init();
	if (handle)
	{
#ifdef _DEBUG
		std::cout << "using save toggle" << std::endl;
#endif
		struct curl_slist* header = nullptr;
		std::string sheader = "Authorization: bearer ";
		sheader += this->token;

		header = curl_slist_append(header, sheader.c_str());
		CURLcode gres = curl_global_init(CURL_GLOBAL_ALL);
		if ( gres == CURLE_OK) {
			if (remove == false) {
				curl_easy_setopt(handle, CURLOPT_URL, "https://oauth.reddit.com/api/save");
			}
			else {
				curl_easy_setopt(handle, CURLOPT_URL, "https://oauth.reddit.com/api/unsave");
			}
			curl_easy_setopt(handle, CURLOPT_POST, 1L);
			curl_easy_setopt(handle, CURLOPT_HTTPHEADER, header);

			std::string params = "id=";
			params += fullname;

			curl_easy_setopt(handle, CURLOPT_POSTFIELDS, params.c_str());
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writedat);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, &json);
			curl_easy_setopt(handle, CURLOPT_USERAGENT, this->useragent.c_str());
			curl_easy_setopt(handle, CURLOPT_HEADERDATA, &rheader);
#ifdef _DEBUG
			curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
#endif

			result = curl_easy_perform(handle);
			curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &responsecode);
			curl_easy_cleanup(handle);
			curl_global_cleanup();

#ifdef _DEBUG
			QFIO("savetoggle_data.txt", json);
#endif

#ifdef _DEBUG
			QFIO("savetoggle_header_data.txt", rheader);
#endif
			if (result != CURLE_OK)
			{
				response.http_state = responsecode;
				response.message = curl_easy_strerror(result);

			}
			else {

				response.message = "";
				response.http_state = responsecode;
			}
		}
		else {
			curl_easy_cleanup(handle);
			response.message = curl_easy_strerror(gres);
			response.http_state = -1;
		}

	}
	else {
		response.http_state = -1;
		response.message = "Failed load libcurl handle!";
	}
	return response;
}

State Saver::obtain_token(bool refresh)
{
	CURL* handle;
	CURLcode result;
	std::string json;
	int responsecode;
	State response;

	handle = curl_easy_init();
#ifdef _DEBUG
	std::cout << "getting token..." << std::endl;
#endif
	if (handle)
	{
		CURLcode gres = curl_global_init(CURL_GLOBAL_ALL);
		if ( gres == CURLE_OK) {
			curl_easy_setopt(handle, CURLOPT_URL, initial_url.c_str());
			curl_easy_setopt(handle, CURLOPT_POST, 1L);
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, &json);
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &writedat);
			curl_easy_setopt(handle, CURLOPT_USERAGENT, this->useragent.c_str());
			std::string userpwd = this->client_id + ":" + this->secret;
			curl_easy_setopt(handle, CURLOPT_USERPWD, userpwd.c_str());

			std::string params = "grant_type=";
			if (refresh == false) {
				params += "password&username=";
				params += this->username;
				params += "&password=";
				params += this->password;
				params += "&scope=";
				params += SCOPE;
			}
			else {
				params += "refresh_token&refresh_token=";
				params += this->token;
			}

#ifdef _DEBUG
			std::cout << params << std::endl;
#endif


			curl_easy_setopt(handle, CURLOPT_POSTFIELDS, params.c_str());
#ifdef _DEBUG
			curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
#endif
			result = curl_easy_perform(handle);
			curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &responsecode);
			curl_easy_cleanup(handle);
			curl_global_cleanup();
#ifdef _DEBUG
			QFIO("token_access.txt", json);
#endif

			if (result != CURLE_OK)
			{
				response.http_state = responsecode;
				response.message = curl_easy_strerror(result);
				return response;
			}
			else {


				nlohmann::json parse = nlohmann::json::parse(json);

				try {
					this->token = parse.at("access_token").get<std::string>();
					response.http_state = responsecode;
#ifdef _DEBUG
					std::cout << "Token obtained: " << this->token << std::endl;
#endif
				}
				catch (nlohmann::json::out_of_range&)
				{
					try {
						response.message = parse.at("message").get<std::string>();
						response.http_state = parse.at("error").get<int>();
					}
					catch (nlohmann::json::out_of_range&) {
						try {
							response.http_state = responsecode;
							response.message = parse.at("invalid_grant").get<std::string>();
							response.http_state = -1;
						}
						catch (nlohmann::json::out_of_range& e) {
							response.message = e.what();
							response.http_state = responsecode;

						}
					}
				}
			}
		}
		else {
			curl_easy_cleanup(handle);
			response.message = curl_easy_strerror(gres);
			response.http_state = -1;
		}


	}
	else {
		response.message = "Failed to load libcurl handle!";
		response.http_state = -1;
		return response;
	}
	return response;
}
