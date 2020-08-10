#include "Imgur.hpp"

State ImgurAccess::retrieve_imgur_image(std::string imghash, std::string& URL)
{
	CURL* handle;
	CURLcode result;
	State s;
	std::string rdata, hd; // return rdata and header data, hd

	handle = curl_easy_init();

	if (handle)
	{
		curl_global_init(CURL_GLOBAL_ALL);
		curl_slist* header = nullptr;
		std::string sheader = "Authorization: Client-ID " + imgur_client_id;
		header = curl_slist_append(header, sheader.c_str());
		std::string _URL = "https://api.imgur.com/3/image/" + std::string(imghash);
		curl_easy_setopt(handle, CURLOPT_URL, _URL.c_str());
		curl_easy_setopt(handle, CURLOPT_HTTPHEADER, header);
		curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &writedat);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, &rdata);
#ifdef _DEBUG
		curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
#endif
		curl_easy_setopt(handle, CURLOPT_HEADERDATA, &hd);
		result = curl_easy_perform(handle);

		curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &s.http_state);
		curl_global_cleanup();
		curl_easy_cleanup(handle);
		curl_free(header);

		if (result != CURLE_OK)
		{
			s.message = curl_easy_strerror(result);
		}
		else {
			nlohmann::json root;
			try {
				root = nlohmann::json::parse(rdata);

				URL = root.at("data").at("link").get<std::string>();
				s.http_state = root.at("status").get<int>();
				s.message = "";
			}
			catch (nlohmann::json::exception& e) {
				s.message = e.what();
				s.http_state = root.at("status").get<int>();
			}
		}

	}
	else {
		s.message = "Failed to load libcurl handle!";
		s.http_state = -1;
	}
	return s;
}

State ImgurAccess::retrieve_album_images(std::string albumhash, std::vector<std::string>& URLs)
{
	State s;
	CURL* handle;
	CURLcode result;
	std::string rdata, hd;

	handle = curl_easy_init();

	if (handle)
	{
		curl_global_init(CURL_GLOBAL_ALL);
		struct curl_slist* header = nullptr;
		std::string sheader = "Authorization: Client-ID " + this->imgur_client_id;
		header = curl_slist_append(header, sheader.c_str());

		std::string URL = "https://api.imgur.com/3/album/" + albumhash;
		curl_easy_setopt(handle, CURLOPT_URL, URL.c_str());
		curl_easy_setopt(handle, CURLOPT_HTTPHEADER, header);
		curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &writedat);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, &rdata);
#ifdef _DEBUG
		curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
#endif
		curl_easy_setopt(handle, CURLOPT_HEADERDATA, &hd);

		result = curl_easy_perform(handle);
		curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &s.http_state);
		curl_easy_cleanup(handle);
		curl_global_cleanup();
		curl_slist_free_all(header);

		if (result != CURLE_OK)
		{
			s.message = curl_easy_strerror(result);
		}
		else {
			nlohmann::json root;
			try {
				root = nlohmann::json::parse(rdata);
				nlohmann::json images = root.at("data").at("images");
				for (auto& elem : images)
				{
					URLs.push_back(elem.at("link").get<std::string>());
				}
			}
			catch (nlohmann::json::exception& e) {
				s.message = e.what();
				s.http_state = -1;
				return s;
			}
			s.message = "";
		}
	}
	else {
		s.http_state = -1;
		s.message = "Failed to initialize libcurl handle!";
	}
	return s;
}

State ImgurAccess::download_item(const char* URL, std::string& buf)
{

	//std::cout << "Retrieving Imgur album" << std::endl;
	CURL* handle;
	CURLcode result;
	State s;
	std::string hd;

	handle = curl_easy_init();
	if (handle)
	{
		curl_global_init(CURL_GLOBAL_ALL);
		curl_easy_setopt(handle, CURLOPT_URL, URL);
		curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &writedat);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, buf);
		curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(handle, CURLOPT_HEADERDATA, &hd);
#ifdef _DEBUG
		curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
#endif

		result = curl_easy_perform(handle);
		curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &s.http_state);
		curl_easy_cleanup(handle);

		if (result != CURLE_OK)
		{
			s.message = curl_easy_strerror(result);
		}
	}
	else {
		s.http_state = -1;
		s.message = "Failed to initialize libcurl handle!";
	}
	return s;
}
