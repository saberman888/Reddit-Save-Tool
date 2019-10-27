#include "Reddit.hpp"
#include <iostream>

void RedditAccess::init_logs() {

	time_t t;
	time(&t);
	char datestr[11];

#if defined(_MSC_VER)
	struct tm timeinfo;
	localtime_s(&timeinfo, &t);
	std::strftime(datestr, sizeof(datestr), "%Y-%m-%d ", &timeinfo);
#else
	struct tm* timeinfo = nullptr;
	timeinfo = localtime(&t);
	std::strftime(datestr, sizeof(datestr), "%Y-%m-%d ", timeinfo);
#endif


#if defined(USE_HOME_DIR)
	char* homedir = getenv("HOME");
	if(homedir == NULL)
		homedir = getpwuid(getuid())->pw_dir;

	this->mediapath = std::string(homedir) + "/RSA/media/" + std::string(datestr) + "/" + Account->username + "/";
	this->logpath = std::string(homedir) + "/RSA/logs/" + std::string(datestr) + "/" + Account->username + "/";
#else
	this->logpath = std::string(fs::current_path().u8string()) + "/logs/" + std::string(datestr) + "/" + Account->username + "/";
	this->mediapath = std::string(fs::current_path().u8string()) + "/media/" + std::string(datestr) + "/" + Account->username + "/";
#endif
	std::clog << "Current log to be generated at: " << this->logpath << std::endl;

	fs::create_directories(logpath);

	log = new std::fstream(logpath + Account->username + "_" + datestr + " info.log", std::ios::out);
	old_rdbuf = std::clog.rdbuf();
	std::clog.rdbuf(log->rdbuf());

	std::clog << "Log generated at " << std::string(datestr) << std::endl;
	std::clog << "Beginning log." << std::endl;
}

RedditAccess::RedditAccess() : log(nullptr), is_logged_in(false), requests_done(0), request_done_in_current_minute(0)
{
}

RedditAccess::~RedditAccess()
{
	if(is_logged_in){
		std::clog.rdbuf(old_rdbuf);
		log->close();
	}
}

bool RedditAccess::load_login_info()
{
	bool success = false;
	std::fstream info("settings.json", std::ios::in | std::ios::out);
	if (!info.good())
	{
		info.open("settings.json", std::ios::out);
		std::clog << "Failed to open settings.json" << std::endl;
		nlohmann::json creds;
		creds["accounts"] = nlohmann::json::array();
		nlohmann::json account = {{ "client_id" ,"cid_here" }, { "secret","secret_here" }, { "username" , "username_here" }, { "password","password_here" }, { "user_agent", "useragent_here" }};
		creds["accounts"].push_back(account);
		creds["imgur"] = {{"client_id","ac_here"}};
		info << creds.dump(4);
		std::cout << "Could not find settings.json" << std::endl;
		std::clog << "Recreating settings.json" << std::endl;
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
				std::clog << e.what() << std::endl;
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
				std::clog << "Username for: " << args.username << " is not in settings.json" << std::endl;
				std::clog << "No username provided, using account of index: " << index << ", username of ";
				this->Account = accounts[index];
				std::clog << this->Account->username << std::endl;
				return true;
			}
			this->Account = accounts[index];
			std::clog << "Account " << Account->username << "is loaded." << std::endl;
		}
		else {
			this->Account = accounts[0];
			std::clog << "Account: " << this->Account->username << " loaded." << std::endl;
		}
		is_logged_in = true;


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
		#ifdef _WIN32
		Sleep(60000);
		#elif _unix
		usleep(60);
		#else
		usleep(60);
		#endif
		restart_minute_clock();
		} else if (mnow >= mthen) {
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

State RedditAccess::retrieve_imgur_image(std::string imghash, std::string& URL)
{
	CURL* handle;
	CURLcode result;
	State s;
	std::string rdata, hd; // return rdata and header data, hd

	handle = curl_easy_init();

	if(handle)
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

		#ifdef _DEBUG
		QFIO(this->logpath + std::string("/imgur_image_" + imghash + std::string(".txt")), rdata);
		#endif

		QFIO(this->logpath + std::string("/imgur_header_") + imghash + std::string(".txt"), hd);

		if(result != CURLE_OK)
		{
			s.message = curl_easy_strerror(result);
		} else {
			nlohmann::json root;
			try {
				root = nlohmann::json::parse(rdata);

				URL = root.at("data").at("link").get<std::string>();
				s.http_state = root.at("status").get<int>();
				s.message = "";
			} catch(nlohmann::json::exception& e) {
				s.message = e.what();
				s.http_state = root.at("status").get<int>();
			}
		}

	} else {
		s.message = "Failed to load libcurl handle!";
		s.http_state = -1;
	}
	return s;
}

State RedditAccess::retrieve_album_images(std::string albumhash, std::vector<std::string>& URLs)
{
	State s;
	CURL* handle;
	CURLcode result;
	std::string rdata, hd;

	handle = curl_easy_init();

	if(handle)
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

		 #ifdef _DEBUG
		 JQFIO(std::string("imgur_album_") + albumhash + ".txt", rdata);
		 #endif

		 if(result != CURLE_OK)
		 {
			 s.message = curl_easy_strerror(result);
		 } else {
			 nlohmann::json root;
			 try {
			 	root = nlohmann::json::parse(rdata);
				nlohmann::json images = root.at("data").at("images");
				for(auto& elem : images)
				{
					URLs.push_back(elem.at("link").get<std::string>());
				}
		 	} catch(nlohmann::json::exception& e) {
				s.message = e.what();
				s.http_state = -1;
				return s;
			}
			s.message = "";
		 }
	} else {
		s.http_state = -1;
		s.message = "Failed to initialize libcurl handle!";
	}
	return s;
}

State RedditAccess::download_item(const char* URL, std::string dest, std::string fn)
{

	//std::cout << "Retrieving Imgur album" << std::endl;
	CURL* handle;
	CURLcode result;
	State s;
	std::string rdata, hd;

	handle = curl_easy_init();
	if(handle)
	{
		curl_global_init(CURL_GLOBAL_ALL);
		curl_easy_setopt(handle, CURLOPT_URL, URL);
		curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &writedat);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, &rdata);
		curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(handle, CURLOPT_HEADERDATA, &hd);
		#ifdef _DEBUG
		curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
		#endif

		result = curl_easy_perform(handle);
		curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &s.http_state);
		curl_easy_cleanup(handle);

		if(result != CURLE_OK)
		{
			s.message = curl_easy_strerror(result);
		} else {
			//check if the destination exists
			if(!fs::exists(dest))
				fs::create_directories(dest);
			// store image
			std::fstream out(dest + "/" + fn, std::ios::out);
			out << rdata;
		}

		s.message = "";
	} else {
		s.http_state = -1;
		s.message = "Failed to initialize libcurl handle!";
	}
	return s;
}
