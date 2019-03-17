#include "Saver.hpp"
#include <iostream>

void QFIO(std::string filename, std::string data)
{
	std::ofstream(filename.c_str(), std::ios::out) << data; 
}

void JQFIO(std::string filename, std::string json)
{
	nlohmann::json  data;
		try {
		data = nlohmann::json::parse(json);

		std::ofstream(filename.c_str(), std::ios::out) << std::setw(4) << data;
	}
	catch (nlohmann::json::parse_error&) {
		std::ofstream(filename.c_str(), std::ios::out) << json;
	}
}

size_t writedat(char* buffer, size_t size, size_t nmemb, std::string& src)
{
	for (size_t i = 0; i < size * nmemb; i++)
	{
		src.push_back(buffer[i]);
	}
	return size * nmemb;
}

std::map<std::string, std::string> MapHeaders(std::string source)
{
	std::istringstream response(source);
	std::string header;
	std::string::size_type index;
	std::map<std::string, std::string> m;
	while (std::getline(response, header) && header != "\r") {
		index = header.find(':', 0);
		if (index != std::string::npos) {
			m.insert(std::make_pair(
				boost::algorithm::trim_copy(header.substr(0, index)),
				boost::algorithm::trim_copy(header.substr(index + 1))
			));
		}
	}
	return m;
}


Saver::Saver(std::string username, std::string password, std::string client_id, std::string secret, std::string useragent)
{
	this->username = username;
	this->password = password;
	this->client_id = client_id;
	this->secret = secret;
	this->useragent = useragent;
	this->use_old_reddit = false;
	this->list_linked_urls = false;
}

State Saver::Save(std::string fullname)
{
	is_mtime_up();
	if (!is_time_up())
	{
		return SaveToggle(fullname, false);
	}
	else {
		State res = obtain_token(true);
		if (res.http_state != 200) {
			return res;
		}
		else {
			return SaveToggle(fullname, false);
		}
	}
}

State Saver::UnSave(std::string fullname)
{
	is_mtime_up();
	if (!is_time_up())
	{
		return SaveToggle(fullname, true);
	}
	else {
		State res = obtain_token(true);
		if (res.http_state != 200) {
			return res;
		}
		else {
			return SaveToggle(fullname, true);
		}
	}
}

State Saver::get_saved_items(std::vector<Item*>& sitem)
{
	CURL *handle;
	CURLcode result;
	State response;
	struct curl_slist* header = nullptr;
	std::string jresponse, hresponse;
	int response_code;

	handle = curl_easy_init();

	if (handle) {
		std::string authorization_header = "Authorization: bearer ";
		authorization_header += token;

		header = curl_slist_append(header, authorization_header.c_str());

		CURLcode gres = curl_global_init(CURL_GLOBAL_ALL);
		if (gres != CURLE_OK) {
			curl_easy_cleanup(handle);
			response.message = curl_easy_strerror(gres);
			response.http_state = -1;
		}
		else {
			std::string url = "https://oauth.reddit.com/user/";
			url += username + "/saved/?limit=10";


			curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
			curl_easy_setopt(handle, CURLOPT_HTTPHEADER, header);
			curl_easy_setopt(handle, CURLOPT_USERAGENT, useragent.c_str());
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, &jresponse);
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &writedat);
			curl_easy_setopt(handle, CURLOPT_HEADERDATA, &hresponse);

#ifdef _DEBUG
			curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
#endif
			result = curl_easy_perform(handle);
			curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response_code);
			curl_easy_cleanup(handle);
			curl_global_cleanup();


#ifdef _DEBUG
			QFIO("saved_header_data.txt", hresponse);
			JQFIO("saved_json_data.txt", jresponse);
#endif

			if (result != CURLE_OK)
			{
				response.http_state = response_code;
				response.message = curl_easy_strerror(result);
			}
			else {
				try {
					nlohmann::json root = nlohmann::json::parse(jresponse);
					auto children = root.at("data").at("children");
					if (root.at("data").at("after").is_null())
						after = "";
					else
						after = root.at("data").at("after").get<std::string>();

					std::string url;
					std::string fullname;
					std::string id;
					std::string kind;
					for (auto& elem : children)
					{
						Item* it = new Item;
						

						fullname = elem.at("data").at("name").get<std::string>();
						kind = elem.at("kind").get<std::string>();
						url = elem.at("data").at("permalink").get<std::string>();

						if (kind == "t3")
							it->is_comment = false;
						else if (kind == "t1")
							it->is_comment = true;
						try {
							it->is_self = elem.at("data").at("is_self").get<bool>();
						}
						catch (nlohmann::json::out_of_range) {
							it->is_self = false;
						}

						if (!it->is_self && !it->is_comment)
							it->link_url = elem.at("data").at("url").get<std::string>();

						try {
							it->domain = elem.at("data").at("domain").get<std::string>();
						}
						catch (nlohmann::json::out_of_range) {
							it->domain = "";
						}
						it->permalink = url;
						it->fullname = fullname;
						it->id = elem.at("data").at("id").get<std::string>();

#ifdef _DEBUG
						std::cout << "Saved item: " << url << ", kind: " << kind << ", fullname" << fullname << std::endl;
#endif

						sitem.push_back(it);
					}

				}
				catch (nlohmann::json::exception & e) {
#ifdef _DEBUG
					std::cout << e.what() << std::endl;
#endif
					response.http_state = -1;
					response.message = e.what();
					return response;
				}
				response.http_state = response_code;
				response.message = "";

			}
		}
	}
	else {
		response.message = "Failed to load libcurl handle!";
		response.http_state = -1;
	}
	return response;
}

void Saver::restart_minute_clock()
{
	std::chrono::minutes one_minute(1);
	mnow = std::chrono::system_clock::now();
	mthen = mnow + one_minute;
	request_done_in_current_minute = 0;
}

void Saver::is_mtime_up()
{
	if ((request_done_in_current_minute == RQ_PER_MINUTE)) {
#ifdef _DEBUG
		std::cout << "60 requests limit per minute has hit, stalling." << std::endl;
#endif
		// Stall then reset time
		while (mnow != mthen) {
			// stall

			if (mnow >= mthen)
				restart_minute_clock(); break;
		}
	}
	else if (mnow >= mthen) {
		restart_minute_clock();
	}
}

bool Saver::backup_as_json(std::string filename, std::vector<Item*>& src)
{
	nlohmann::json root = nlohmann::json::array();
	for (Item* elem : src)
	{
		nlohmann::json item;
		item["is_self"] = elem->is_self;
		item["is_comment"] = elem->is_comment;
		item["permalink"] = elem->permalink;
		item["id"] = elem->id;
		item["fullname"] = elem->fullname;
		item["linked_url"] = elem->link_url;
		item["domain"] = elem->domain;

		root.push_back(item);
	}

	std::ofstream out(filename, std::ios::out);
	if (!out.good())
		return false;
	out << std::setw(4) << root;
	return true;
}

bool Saver::output_simple_format(std::string filename, std::vector<Item*>& src)
{
	std::ofstream out(filename, std::ios::out);
	if(!out.good())
		return false;
	for (Item* elem : src)
	{
		std::string url, linked_url;
		if (use_old_reddit)
			url = "https://old.reddit.com" + elem->permalink;
		else
			url = "https://www.reddit.com" + elem->permalink;

		if(!elem->is_self)
			linked_url = elem->link_url;

		out << url << std::endl;
		
		if (elem->domain == "reddit.com" && use_old_reddit)
		{
#ifdef _DEBUG
			std::cout << "Domain: reddit.com" << std::endl;
#endif
			std::string new_linked_url = linked_url.substr(22, linked_url.size());
#ifdef _DEBUG
			std::cout << "String substringed as: " << new_linked_url << std::endl;
#endif
			linked_url = "https://old.reddit.com" + new_linked_url;
		}

		if (!elem->is_self && this->list_linked_urls) {
#ifdef _DEBUG
			std::cout << "Linked URL: " << linked_url << std::endl;
#endif
			out << linked_url << std::endl;
		}
	}
	return true;
}

State Saver::AccessSaved(std::vector<Item*>& saved)
{
	is_mtime_up();
	if (!is_time_up())
	{
		return get_saved_items(saved);
	}
	else {
		State res = obtain_token(true);
		if (res.http_state != 200)
		{
			return res;
		}
		else {
			return get_saved_items(saved);
		}
	}
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

					now = std::chrono::system_clock::now();
					std::chrono::hours one_hour(1);

					then = now + one_hour;

					std::time_t t = std::chrono::system_clock::to_time_t(then);
					char stime[26];

					ctime_s(stime, sizeof stime, &t);
#ifdef _DEBUG
					std::cout << "Token will expire at: " << stime << std::endl;
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
