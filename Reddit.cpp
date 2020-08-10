#include "Reddit.hpp"
#include <iostream>

RedditAccess::RedditAccess() : IsLoggedIn(false), requests_done(0), request_done_in_current_minute(0)
{
	if (IsUnixBased)
	{
		// TODO: Give an option to store in anywhere other than the home directory
		StoragePath = std::string(getenv("HOME")) + "/Reddit/";
	}
	else {
		StoragePath = std::string::empty;
	}
}

bool RedditAccess::LoadLogins()
{
	bool success = false;
	std::fstream info("settings.json", std::ios::in | std::ios::out);
	if (!info.good())
	{
		info.open("settings.json", std::ios::out);
		nlohmann::json creds;
		creds["accounts"] = nlohmann::json::array();
		nlohmann::json account = {{ "client_id" ,"cid_here" }, { "secret","secret_here" }, { "username" , "username_here" }, { "password","password_here" }, { "user_agent", "useragent_here" }};
		creds["accounts"].push_back(account);
		creds["imgur"] = {{"client_id","ac_here"}};
		info << creds.dump(4);
		return success;
	}


	nlohmann::json root;

	try {
		info >> root;
		size_t acc_size = root.at("accounts").size();
		for (nlohmann::json& elem : root.at("accounts"))
		{
			try {
				struct creds* acs = new struct creds;
				acs->username = elem.at("username").get<std::string>();
				acs->password = elem.at("password").get<std::string>();
				acs->client_id = elem.at("client_id").get<std::string>();
				acs->secret = elem.at("secret").get<std::string>();
				acs->user_agent = elem.at("user_agent").get<std::string>();


				this->accounts.push_back(acs);
				}
			catch (nlohmann::json::out_of_range& e) {
			}
		}
		if (args.username != "") {
			int index = 0;
			bool found = false;
			for (size_t i = 0; i < acc_size; i++) {
				if (std::string usr = root.at("accounts")[i].at("username").get<std::string>(); usr == args.username){
					index = i;
					found = true;
				}
			}

			if (!found) {
				this->Account = accounts[index];
				return true;
			}
			this->Account = accounts[index];
		}
		else {
			this->Account = accounts[0];
		}
		IsLoggedIn = true;


		try {
			auto imgur = root.at("imgur");

			this->imgur_client_id = imgur.at("client_id").get<std::string>();
			this->imgur_enabled = true;
			std::cout << "Imgur support loaded." << std::endl;
		} catch(nlohmann::json::exception&) {
			this->imgur_enabled = false;
			std::cout << "Warning: Imgur albums will only be downloaded as a zip." << std::endl;
		}
		success = true;
	}
	catch (nlohmann::json::exception& e) {
		success = false;
	}

	return success;
}

State RedditAccess::obtain_token(bool refresh)
{
	CURL* handle;
	CURLcode result;
	std::string json;
	State response;

	handle = curl_easy_init();
#ifdef _DEBUG
	std::cout << "getting token..." << std::endl;
#endif
	if (handle)
	{
		CURLcode gres = curl_global_init(CURL_GLOBAL_ALL);
		if (gres == CURLE_OK) {
			curl_easy_setopt(handle, CURLOPT_URL, "https://www.reddit.com/api/v1/access_token");
			curl_easy_setopt(handle, CURLOPT_POST, 1L);
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, &json);
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &writedat);
			curl_easy_setopt(handle, CURLOPT_USERAGENT, Account->user_agent.c_str());
			std::string userpwd = Account->client_id + ":" + Account->secret;
			curl_easy_setopt(handle, CURLOPT_USERPWD, userpwd.c_str());

			std::string params = "grant_type=";
			if (refresh == false) {
				params += "password&username=" + Account->username + "&password=";
				params += Account->password + "&scope=" +"%20save%20history%20read";
			}
			else {
				params += "refresh_token&refresh_token=" + this->token;
			}

			curl_easy_setopt(handle, CURLOPT_POSTFIELDS, params.c_str());
#ifdef _DEBUG
			curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
#endif
			result = curl_easy_perform(handle);
			curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response.http_state);
			

			if (result != CURLE_OK)
			{
				response.message = curl_easy_strerror(result);
				return response;
			}
			else {

				nlohmann::json parse;

				try {
                    parse = nlohmann::json::parse(json);
					this->token = parse.at("access_token").get<std::string>();
#ifdef _DEBUG
					std::cout << "Token obtained: " << this->token << std::endl;
#endif

					now = std::chrono::system_clock::now();
					std::chrono::hours one_hour(1);

					then = now + one_hour;

					std::time_t t = std::chrono::system_clock::to_time_t(then);
					char stime[26];
#if defined(_MSC_VER)
					ctime_s(stime, sizeof stime, &t);
#else
					ctime_r(&t, stime);
#endif

#ifdef _DEBUG
					std::cout << "Token will expire at: " << stime << std::endl;
#endif
				}
				catch (nlohmann::json::out_of_range&)
				{
					try {
						response.message = parse.at("message").get<std::string>();
					}
					catch (nlohmann::json::out_of_range&) {
						try {
							response.message = parse.at("invalid_grant").get<std::string>();
							response.http_state = -1;
						}
						catch (nlohmann::json::out_of_range& e) {
							response.message = e.what();
							response.http_state = -1;

						}
					}
				}
			}
			curl_easy_cleanup(handle);
			curl_global_cleanup();
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

void RedditAccess::restart_minute_clock()
{
	std::chrono::minutes one_minute(1);
	mnow = std::chrono::system_clock::now();
	mthen = mnow + one_minute;
	request_done_in_current_minute = 0;
}

void RedditAccess::is_mtime_up()
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
		} else if (mnow >= mthen) {
		restart_minute_clock();
	}
}

void RedditAccess::tick()
{
	requests_done += 1;
	if (now < then) {
		this->request_done_in_current_minute += 1;
	}
}

