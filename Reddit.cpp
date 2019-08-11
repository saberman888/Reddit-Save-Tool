#include "Reddit.hpp"
#include <iostream>

void RedditAccess::init_logs() {

	time_t t;
	struct tm* timeinfo = nullptr;

	time(&t);
#if defined(_MSC_VER)
	localtime_s(&timeinfo, &t);
#elif (defined(__MINGW64__) || defined(__MINGW32__))
	timeinfo = localtime(&t);
#else 
	timeinfo = localtime(&t);
#endif
	char datestr[7];

	std::strftime(datestr, sizeof(datestr), "%Y_%m_%d ", timeinfo);

#ifdef __WIN32__
	this->logpath = "logs\\" + std::string(datestr) + "\\" + Account->username + "\\";
	this->mediapath = "media\\" + std::string(datestr) + "\\" + Account->username + "\\";
#else
	this->logpath = std::string(fs::current_path()) + "/logs/" + std::string(datestr) + "/" + Account->username + "/";
	this->mediapath = std::string(fs::current_path()) + "/media/" + std::string(datestr) + "/" + Account->username + "/";
#endif
	std::clog << "Current log to be generated at: " << this->logpath << std::endl;

	fs::create_directories(logpath);

	log = new std::fstream(logpath + Account->username + "_" + datestr + " info.log", std::ios::out);

	old_rdbuf = std::clog.rdbuf();
	std::clog.rdbuf(log->rdbuf());

	std::clog << "Log generated at " << std::string(datestr) << std::endl;
	std::clog << "Beginning log." << std::endl;
}

RedditAccess::RedditAccess(CMDArgs* arg) : args(arg)
{
	// TODO: Give RSA it's own general log
}

RedditAccess::~RedditAccess()
{
	std::clog.rdbuf(old_rdbuf);
	log->close();
	delete args;
}

bool RedditAccess::load_login_info()
{
	bool success = false;
	std::fstream info("settings.json", std::ios::in | std::ios::out);
	if (!info.good()) {
		std::clog << "Failed to open settings.json" << std::endl;
		nlohmann::json creds = { {"accounts", nullptr } };
		creds["accounts"] = nlohmann::json::array({ {"client_id" , "cid" }, {"secret","secret"}, {"username" , "user"}, {"password","pass"}, {"user_agent", "ua"} });
		info << creds.dump(4);
		std::clog << "Recreating settings.json" << std::endl;
		return false;
	}


	nlohmann::json root;

	try {
		info >> root;
		int acc_size = root.at("accounts").size();
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
				std::clog << e.what() << std::endl;
			}
		}
		if (args->username != "") {
			//std::clog << "Checking if " << args->username << " is in settings.json" << std::endl;
			int index = 0;
			bool found = false;
			for (int i = 0; i < acc_size; i++) {
				if (std::string usr = root.at("accounts")[i].at("username").get<std::string>(); usr == args->username){
					index = i; 
					found = true;
				}
			}

			if (!found) {
				std::clog << "Username for: " << args->username << " is not in settings.json" << std::endl;
				std::clog << "No username provided, using account of index: 0" << std::endl;
				this->Account = accounts[index];
				return false;
			}
			this->Account = accounts[index];
			std::clog << "Account loaded." << std::endl;
		}
		else {
			this->Account = accounts[0];
			std::clog << "Account: " << this->Account->username << " loaded." << std::endl;
		}

		success = true;
	}
	catch (nlohmann::json::exception& e) {
		std::clog << e.what() << std::endl;
		std::clog << "Failed to load credentials" << std::endl;
		success = false;
	}
	
	return success;
}

State RedditAccess::obtain_token(bool refresh)
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
						std::clog << "Checking error.." << std::endl;
						response.message = parse.at("message").get<std::string>();
						response.http_state = parse.at("error").get<int>();
					}
					catch (nlohmann::json::out_of_range&) {
						try {
							std::clog << "Checking if error is an invalid grant" << std::endl;
							response.http_state = responsecode;
							response.message = parse.at("invalid_grant").get<std::string>();
							response.http_state = -1;
						}
						catch (nlohmann::json::out_of_range& e) {
							std::clog << "Unknown error from obtain_token" << std::endl;
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
		std::clog << "60 requests limit per minute has hit, stalling." << std::endl;
		// Stall then reset time
		while (mnow != mthen) {
			// stall

			if (mnow >= mthen){
				restart_minute_clock(); break;
			}
		}
	}
	else if (mnow >= mthen) {
		std::clog << "Restarting the minute clock!" << std::endl;
		restart_minute_clock();
	}
}

void RedditAccess::tick()
{
	requests_done += 1;
	if (now < then) {
		this->request_done_in_current_minute += 1;
	}

	std::clog << "Requests done: " << requests_done << std::endl;
	std::clog << "Requests done in current minute: " << this->request_done_in_current_minute << std::endl;
}
