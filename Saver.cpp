#include "Saver.hpp"
#include <iostream>


bool Saver::LoadLogins()
{
	bool success = false;
	std::fstream input("settings.json", std::ios::in | std::ios::out);
	if (!input.good())
	{
		// If the config file containing accounts is not present, then write one with dummy values so the end user can fill it in
		input.open("settings.json", std::ios::out);
		nlohmann::json settingsfilling =
		{
			{"accounts", {"client_id", "CLIENT_ID_HERE"},
			{"secret", "SECRET_HERE"},
			{"username", "USERNAME_HERE"},
			{"password", "PASSWORD_HERE"},
			{"useragent", "USERAGENT_HERE"}},
			{"imgur_client_id", "IMGUR_CLIENT_ID_HERE"}
		};

		input << settingsfilling.dump(4);
		return success;
	}

	// Read the config file into root
	nlohmann::json root;
	input >> root;
	if (root.contains("accounts"))
	{
		nlohmann::json account;
		if (!args.username.empty())
		{
			for (int i = 0; i < root.at("accounts").size(); i++)
			{
				if (args.username == account.at("username"))
				{
					account = root.at("accounts")[i];
				}
			}
		}
		else {
			account = root.at("")
		}
		else {

		}
	}
	else {
		std::cerr << "Error, accounts array is not present" << std::endl;
		return success;
	}

	try {
		input >> root;
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
				if (std::string usr = root.at("accounts")[i].at("username").get<std::string>(); usr == args.username) {
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
		}
		catch (nlohmann::json::exception&) {
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


State Saver::get_saved_items(std::vector< Item* >& sitem, std::string after)
{
	std::string URL =
		"https://oauth.reddit.com/user/"
		+ UserAccount.Username
		+ "/saved/?limit="
		+ std::to_string(100)
		+"&after=" + after;
	RedditHandle.Setup(URL);
			else {

				std::clog << "Attempting to parse json structures for saved items" << std::endl;
				try {
					nlohmann::json root = nlohmann::json::parse(jresponse);
					auto children = root.at("data").at("children");
					if (root.at("data").at("after").is_null())
						this->after = "";
					else
						this->after = root.at("data").at("after").get<std::string>();

#ifdef _DEBUG
					std::cout << "After: " << this->after << std::endl;
#endif

					std::clog << "The after is: " << this->after << std::endl;

					for(size_t j = 0; j < children.size(); j++)
					{
						auto& elem = children[j];
						Item* it = new Item;
						it->kind = elem.at("kind").get<std::string>();

						if (it->kind == "t1") {
							std::clog << "Item is a comment" << std::endl;
							it->fullname = elem.at("data").at("link_id").get<std::string>();
							it->url = elem.at("data").at("link_url").get<std::string>();
							it->is_self = false;
							try {
								it->is_video = elem.at("data").at("is_video").get<bool>();
							}
							catch (nlohmann::json::out_of_range& e) {
								it->is_video = false;
							}
							it->self_text = "";
							it->domain = "";
							std::string title = elem.at("data").at("link_title").get<std::string>();
							boost::replace_all(title, ",", "&#44;");
							boost::replace_all(title, "\"", "&#34;");
							it->title = title;
							it->orig_body = elem.at("data").at("body").get<std::string>();

                            boost::replace_all(it->body, ",", "&#44;");
                            boost::replace_all(it->body, "\"", "&#34;");
                            boost::replace_all(it->body, "\n", "&#10;");
                            boost::replace_all(it->body, "\r", "&#13;");

							it->body = "\"" + it->body + "\"";

							std::string permalink = elem.at("data").at("permalink").get<std::string>();
							it->permalink = permalink;


							std::string id = elem.at("data").at("link_id").get<std::string>();
							id = id.substr(3, id.size());

                            //it->depth = elem.at("data").at("depth").get<int>();

							it->id = id;
						}
						else if (it->kind == "t3") {
							std::clog << "Item is a post" << std::endl;
							it->fullname = elem.at("data").at("name").get<std::string>();
							it->url = elem.at("data").at("url").get<std::string>();
							it->is_self = elem.at("data").at("is_self").get<bool>();
							try {
								it->is_video = elem.at("data").at("is_video").get<bool>();

                                nlohmann::json rvideo = elem.at("data").at("media").at("reddit_video");


								it->fallback_url = rvideo.at("fallback_url").get<std::string>();
								it->is_gif = rvideo.at("is_gif").get<bool>();

								std::size_t size = it->fallback_url.substr(it->fallback_url.find("DASH")).size();
								std::string audio_result = it->fallback_url.substr(0, it->fallback_url.size()-(int)size);
								it->audio_url = audio_result + "audio";

								#ifdef _DEBUG
								std::clog << "fallback URL for: " << it->id << " is " << it->fallback_url << std::endl;
								std::clog << "With an Audio url for: " << it->audio_url << std::endl;

								std::cout << "fallback URL for: " << it->id << " is " << it->fallback_url << std::endl;
								std::cout << "With an Audio url for: " << it->audio_url << std::endl;
								#endif
							}
							catch (nlohmann::json::exception& e) {
								it->is_video = false;
							}
							if (it->is_self)
							{
								it->orig_self_text = elem.at("data").at("selftext").get<std::string>();
								it->self_text = it->orig_self_text;

                                boost::replace_all(it->self_text, ",", "&#44;");
                                boost::replace_all(it->self_text, "\"", "&#34;");
                                boost::replace_all(it->self_text, "\'", "&#40;");
                                boost::replace_all(it->self_text, "\n", "&#10;");
                                boost::replace_all(it->self_text, "\r", "&#13;");
							}
							it->domain = elem.at("data").at("domain").get<std::string>();
							std::string title = elem.at("data").at("title").get<std::string>();
							boost::replace_all(title, ",", "&#44;");
							boost::replace_all(title, "\"", "&#34;");
							boost::replace_all(title, "\'", "&#40;");
							it->title = title;
							it->permalink = elem.at("data").at("permalink").get<std::string>();

							it->id = elem.at("data").at("id").get<std::string>();
						}
						if (!elem.at("data").at("author").is_null())
							it->author = elem.at("data").at("author").get<std::string>();
						else
							it->author = "[deleted]";
						std::clog << "ID: " << it->id << std::endl;
						it->created_utc = elem.at("data").at("created_utc").get<long>();
						it->subreddit = elem.at("data").at("subreddit").get<std::string>();
						boost::algorithm::to_lower(it->subreddit);
						it->num_comments = elem.at("data").at("num_comments").get<int>();
						it->over_18 = elem.at("data").at("over_18").get<bool>();
						it->score = elem.at("data").at("score").get<int>();
						it->stickied = elem.at("data").at("stickied").get<bool>();
						it->subreddit_id = elem.at("data").at("subreddit_id").get<std::string>();

						// replace permalink if
						if (args.RHA) {
							it->rha_permalink = it->permalink;
							it->rha_permalink.replace(0, it->subreddit.size() + 3, "/r/" + Account->username);
						}

						sitem.push_back(it);
						std::clog << it->permalink << std::endl;
						std::cout << it->permalink << std::endl;
					}

				}
				catch (nlohmann::json::exception & e) {
#ifdef _DEBUG
					std::cout << e.what() << std::endl;
#endif

					std::clog << e.what() << std::endl;
					response.http_state = -1;
					response.message = e.what();
					std::clog << "get_saved_items result state: " << response.http_state << ", " << response.message << std::endl;
					return response;
				}
			}
		}
	}
	else {
		response.message = "Failed to load libcurl handle!";
		response.http_state = -1;
	}
	std::clog << "get_saved_items result state: " << response.http_state << ", " << response.message << std::endl;
	return response;
}
State Saver::retrieve_comments(Item* i)
{
	std::clog << "retriving comments..." << std::endl;
	CURLcode result;
	CURL* handle;
	std::string jresponse;
	State response;
	struct curl_slist* header = nullptr;

	handle = curl_easy_init();

	if (handle)
	{
		std::clog << "Setting up header and request." << std::endl;
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
			std::string url;
			if(i->kind == "t3")
				url = "https://oauth.reddit.com/r/" + i->subreddit + "/comments/" + i->id + "/?limit=500&showmore=true&depth=500";
			else if(i->kind == "t1")
				url = "https://oauth.reddit.com/r/" + i->subreddit + "/comments/" + i->id + "/?context=1000&depth=500";

			std::clog << "URL: " << url << std::endl;

			curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
			curl_easy_setopt(handle, CURLOPT_HTTPHEADER, header);
			curl_easy_setopt(handle, CURLOPT_USERAGENT, Account->user_agent.c_str());
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, &jresponse);
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &writedat);

			result = curl_easy_perform(handle);
			curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response.http_state);
			curl_easy_cleanup(handle);
			curl_global_cleanup();
			curl_free(header);

			tick();

			if (result != CURLE_OK)
			{
				response.message = curl_easy_strerror(result);
			}
			else {
				JQFIO(logpath + "/comments_" + i->id + ".json", jresponse);
				std::clog << "Parsing comments... " << std::endl;
				nlohmann::json root;

				try {
					root = nlohmann::json::parse(jresponse);
					response.http_state = root.at("error").get<int>();
					response.message = root.at("message").get<std::string>();
					std::clog << "Item: " << response.http_state << ", " << response.message << std::endl;
					return response;
				}
				catch (nlohmann::json::exception&) {

					std::string id;
					nlohmann::json comments, data;
					try {
						comments = root[1];
						data = comments.at("data");

						for (auto& melem : data.at("children"))
						{
							Comment* c = new Comment;
							auto elem = melem.at("data");
							i->kind = melem.at("kind").get<std::string>();
							if( i->kind == "t1"){
								// going to do something with this in the future

								try {
									c->author = elem.at("author").get<std::string>();
								}
								catch (nlohmann::json::out_of_range&) {
									c->author = "[deleted]";
								}
								try {
									std::string body = elem.at("body").get<std::string>();
									boost::replace_all(body, ",", "&#x2c;");
									boost::replace_all(body, "\"", "&#x22;");
									boost::replace_all(body, "\n", "&#13;");
									c->body = "\"" + body + "\"";
								}
								catch (nlohmann::json::out_of_range&) {
									c->body = "[deleted]";
								}
								try {
									c->created_utc = elem.at("created_utc").get<long>();
								}
								catch (nlohmann::json::out_of_range&) {
									c->created_utc = elem.at("created").get<long>();
								}

								c->link_id = elem.at("link_id").get<std::string>();
								c->parent_id = elem.at("parent_id").get<std::string>();
								c->score = elem.at("score").get<int>();
								c->stickied = elem.at("stickied").get<bool>();
								c->subreddit_id = elem.at("subreddit_id").get<std::string>();

								i->comments.push_back(c);
							}
						}
					}
					catch (nlohmann::json::out_of_range& e) {
						std::cerr << e.what() << std::endl;
						std::cerr << "Object ID: " << i->id << std::endl;

						std::clog << e.what() << std::endl;
						std::clog << "Object ID: " << i->id << std::endl;

					}
				}

			}
		}
	}
	else {
		response.message = "Failed to load libcurl handle!";
		response.http_state = -1;
	}
	std::clog << "get_saved_items result state: " << response.http_state << ", " << response.message << std::endl;
	return response;



void Saver::download_content(std::vector<Item*> i)
{
	std::clog << "Beginning to save content." << std::endl;
	std::clog << "Sorted by enum: " << args.sort << std::endl;

	// if the number of posts i is less than of args.limit, replace args.limit with the size of i
	if (i.size() < (unsigned)args.limit)
		args.limit = (int)i.size();

	for (size_t j = 0; j < (unsigned)args.limit; j++) {
		Item *elem = i[j];

		#ifdef _DEBUG
		std::cout << elem->url << std::endl;
		#endif

 		if(std::vector<std::string>::iterator whitelist_it = std::find(std::begin(args.whitelist), std::end(args.whitelist), elem->subreddit); (whitelist_it == std::end(args.whitelist)) && (!args.whitelist.empty()))
		{
			std::cout << "#" << j << " Skipping: " << elem->kind << ", " << elem->id << ", " << elem->permalink << ", " << elem->url << ", " << elem->author << std::endl;
			std::cout << "Reason: Username/Subreddit was on a whitelist" << std::endl;

			std::clog << "Skipping: " << elem->kind << ", " << elem->id << ", " << elem->permalink << ", "<< elem->url << ", " << elem->author << std::endl;
			std::clog << "Reason: Username/Subreddit was on a whitelist" << std::endl;
			continue;
		}

		if(std::vector<std::string>::iterator blacklist_it = std::find(std::begin(args.blacklist), std::end(args.blacklist), elem->subreddit); blacklist_it != std::end(args.blacklist))
		{
			std::clog << "Skipping: " << elem->kind << ", " << elem->id << ", " << elem->permalink << ", "<< elem->url << ", " << elem->author << std::endl;
			std::clog << "Reason: Username/Subreddit was on a blacklist" << std::endl;
			continue;
		}

		if(std::vector<std::string>::iterator user_whitelist_it = std::find(std::begin(args.uwhitelist), std::end(args.uwhitelist), elem->author); (user_whitelist_it == std::end(args.uwhitelist)) && (!args.uwhitelist.empty()))
		{
			std::clog << "Skipping: " << elem->kind << ", " << elem->id << ", " << elem->permalink << ", "<< elem->url << ", " << elem->author << std::endl;
			std::clog << "Reason: Username/Subreddit was on a whitelist" << std::endl;
			continue;
		}

		if(std::vector<std::string>::iterator user_blacklist_it = std::find(std::begin(args.ublacklist), std::end(args.ublacklist), elem->author); (user_blacklist_it != std::end(args.ublacklist)) && (!args.ublacklist.empty()))
		{
			std::clog << "Skipping: " << elem->kind << ", " << elem->id << ", " << elem->permalink << ", "<< elem->url << ", " << elem->author << std::endl;
			std::clog << "Reason: Username/Subreddit was on a blacklist" << std::endl;
			continue;
		}

		if(std::vector<std::string>::iterator domain_whitelist_it = std::find(std::begin(args.dwhitelist), std::end(args.dwhitelist), elem->domain); (domain_whitelist_it == std::end(args.dwhitelist)) && (!args.dwhitelist.empty()))
		{
			std::clog << "Skipping: " << elem->kind << ", " << elem->id << ", " << elem->permalink << ", "<< elem->url << ", " << elem->author << std::endl;
			std::clog << "Reason: Domain was on a whitelist" << std::endl;
			continue;
		}

		if(std::vector<std::string>::iterator domain_blacklist_it = std::find(std::begin(args.dblacklist), std::end(args.dblacklist), elem->domain); (domain_blacklist_it != std::end(args.dblacklist)) && (!args.dblacklist.empty()))
		{
			std::clog << "Skipping: " << elem->kind << ", " << elem->id << ", " << elem->permalink << ", "<< elem->url << ", " << elem->author << std::endl;
			std::clog << "Reason: Domain was on a blacklist" << std::endl;
			continue;
		}

		std::string path = this->mediapath;
		switch (args.sort)
		{
		case Subreddit:
			path += elem->subreddit;
			break;
		case ID:
			path += elem->id;
			break;
		case Title:
		{
			if(elem->title.size() > 255)
				elem->title.resize(255);
			path += stripfname(elem->title);
			break;
		}
		default:
			// Unsorted
			break;
		}
		path += "/";

		if (elem->IsImgurAlbum() && args.EnableImgurAlbums)
		{
			std::vector<std::string> vih, vai;
			boost::split(vih, elem->url, boost::is_any_of("/"));

			std::string hash = vih[4];

			State iam = retrieve_album_images(hash,vai);
			if(iam.http_state != 200)
			{
				std::cout << "Error: Failed to retrieve Imgur album URLs, " << iam.message << std::endl;
				continue;
			}

			if(elem->title.size() > 255)
				elem->title.resize(255);

			std::string dest = path + elem->title;
			std::string fn;
			std::cout << "#" << j << " Retrieving imgur album: " << elem->url << " from " << elem->subreddit << std::endl;
			for(int i = 0; i < vai.size(); i++)
			{
				std::vector<std::string> _vih;
				boost::split(_vih, vai[i], boost::is_any_of("/"));
				fn = std::to_string(i) + "_" + _vih[3];

				State res = download_item(vai[i].c_str(), dest, fn);
				if(res.http_state != 200)
				{
					std::cout << "Error failed to retrieve " << i+1 << " of " << vai.size() << std::endl;
					std::cout << "Reason: " << res.message << std::endl;
				}
				std::cout << "Retrieving: " << i+1 << " of " << vai.size() << std::endl;
			}
			std::cout << std::endl;
		} else if( elem->IsImgurLink() && imgur_enabled) {
			std::string url = elem->url;
			std::clog << "#" << j << " Retrieving Imgur image: " << url << std::endl;
			if(elem->IsPossibleImage()){
				// strip the extension from the urls
				std::string urls[] = {".jpeg", ".bmp", ".png", ".gif", ".jpg", ".tiff" };
				for(std::string elem : urls)
					boost::erase_all(url, elem);
			}
			std::vector<std::string> surl;
			boost::split(surl, url, boost::is_any_of("/"));

			#ifdef _DEBUG
			std::clog << "Imgur hash: " << surl[3] << std::endl;
			std::cout << "Imgur hash: " << surl[3] << std::endl;
			#endif

			State res = retrieve_imgur_image(surl[3], url);
			if(res.http_state != 200)
			{
				std::cout << "Error failed to retrieve imgur image!" << std::endl;
				std::cout << "Reason: " << res.message << std::endl;

				std::clog << "Error failed to retrieve imgur image!" << std::endl;
				std::clog << "Reason: " << res.message << std::endl;
			} else {
				elem->url = url;
			}
		}

        if(elem->is_video && args.VideosEnabled)
		{
			if (!fs::exists(path)) {
				try {
					fs::create_directories(path);
				}
				catch (fs::filesystem_error& e) {
					std::clog << e.what() << ", " << path << ", ID: " << elem->id << std::endl;
					std::cout << e.what() << ", " << path << ", ID: " << elem->id << std::endl;
				}
			}
			std::cout << "#" << j << " Retrieving video: " << elem->url << std::endl;
            // Remove all instances of invalid characters
            std::string fn = stripfname(elem->title);
            boost::replace_all(fn, " ", "_"); // replace all instances of space so a filename can be better made of the title
			// Download both the audio and video and then mux them using ffmpeg
			download_item(elem->fallback_url.c_str(), path, std::string(fn + ".mp4"));
			download_item(elem->audio_url.c_str(), path, std::string(fn + ".mp3"));
			std::string cmd_args = "ffmpeg -y -i " + path + fn + std::string(".mp4") + " -i " +  path + fn + std::string(".mp3") + " -c copy -map 0:v -map 1:a " + path + fn + ".mkv";
            // comeplete the task
			std::cout << cmd_args << std::endl;
            std::clog << "#" << j << " Retrieving video of " << elem->title << std::endl;
			system(cmd_args.c_str());
            // Once done remove both the audio file and video file
            fs::remove(path + std::string(fn + ".mp3"));
            fs::remove(path + std::string(fn + ".mp4"));
			continue;
		}
		CURL* handle;
		CURLcode result;
		State s;
		std::string data, hd, ru;
		char* ct = nullptr;

		handle = curl_easy_init();

		if (handle) {
			curl_global_init(CURL_GLOBAL_ALL);
			curl_easy_setopt(handle, CURLOPT_URL, elem->url.c_str());
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &writedat);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);
			curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(handle, CURLOPT_HEADERDATA, &hd);
#ifdef _DEBUG
			curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
#endif
			result = curl_easy_perform(handle);
			curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &s.http_state);
			curl_easy_getinfo(handle, CURLINFO_CONTENT_TYPE, &ct);
			QFIO(this->logpath + "content_download_" + elem->id + ".txt", hd);

			if(result != CURLE_OK)
			{
				std::cout << "Skipping: " << elem->kind <<", " << elem->id << ", " << elem->subreddit << ", " << elem->url << std::endl;
				std::cerr << "Reason: " << curl_easy_strerror(result) << std::endl;
				continue;
			}

			if (s.http_state == 200) {

				if(args.EnableCommentThreads)
					RetrieveComments(elem);

				std::vector<std::string> res;
				boost::split(res, ct, boost::is_any_of("/"));
				if (args.EnableImages) {
					if ((res[0] == "image") && !fs::exists(path)) {
						try {
							fs::create_directories(path);
						}
						catch (fs::filesystem_error& e) {
							std::clog << e.what() << ", " << path << ", ID: " << elem->id << std::endl;
							std::cout << e.what() << ", " << path << ", ID: " << elem->id << std::endl;
						}
					}

					if (res[0] == "image") {
						std::ofstream(path + elem->id + "." + res[1], std::ios::binary) << data;
						std::clog << "Content: " << elem->id << " stored at " << path << std::endl;
						std::cout << "#" << j <<  " Retrieving content: " << elem->url << " from " << elem->permalink << std::endl;
                        continue;
                    }
				}
				if (args.EnableText) {

					std::clog << "Outputting " << elem->id << ", " << elem->kind << std::endl;
					if ((elem->is_self && elem->kind == "t3") || elem->kind == "t1") {
						if (!fs::exists(path)) {
							try {
								fs::create_directories(path);
							}
							catch (fs::filesystem_error& e) {
								std::clog << e.what() << ", " << path << ", ID: " << elem->id << std::endl;
								std::cout << e.what() << ", " << path << ", ID: " << elem->id << std::endl;
							}
						}
						std::fstream out(path + elem->kind + "_" + elem->id + ".txt", std::ios::out);
						if(elem->kind == "t3")
							out << "Title: " << elem->title << std::endl;
						out << "Author: " << elem->author << std::endl;
						out << "Date: " << to_realtime(elem->created_utc) << std::endl;
						out << elem->permalink << std::endl;
                        if(elem->is_self)
                            out << elem->orig_self_text << std::endl;
                        else
                            out << elem->url;
						if (elem->kind == "t1"){
							out << elem->orig_body << std::endl;

                            /*if(!elem->comments.empty())
                            {
                                TODO: Implement adding child comments to the parent
                            }*/

						}
						else{
							out << elem->orig_self_text << std::endl;
						}
						std::cout << "#" << j << " Retrieving content: " << elem->permalink << std::endl;
                        continue;
					}

				}
			}
			else {
				std::clog << "Message: " << s.message << ", HTTP State: " << s.http_state << std::endl;
				std::clog << "Failed to download: " << elem->id << std::endl;
			}
		}
		else {
			std::clog << "Error: Failed to initialize CURL handle" << std::endl;
		}
	}
}

Saver::Saver() : RedditAccess(), IsLoggedIn(false)
{
	if (IsUnixBased)
	{
		// TODO: Give an option to store in anywhere other than the home directory
		MediaPath = std::string(getenv("HOME")) + "/Reddit/";
	}
	else {
		MediaPath = std::string::empty;
	}
}

bool Saver::scan_cmd(int argc, char* argv[])
{
	for (int i = 1; i < argc; i++)
	{
		std::string arg = argv[i];
		if (arg == "-i") {
			args.EnableImages = false;
		}
		else if (arg == "-t") {
			args.EnableText = false;
		}
		else if (arg == "-a") {
			if (i + 1 >= argc) {
				std::cout << "Error: Secondary argument for -a option not present" << std::endl;
				return false;
			}
			args.username = argv[i + 1];
			i++;
		}
		else if (arg == "-dc") {
			args.DisableComments = true;
		}
		else if (arg == "-rha") {
			args.RHA = true;
		}
		else if (arg == "-h" || arg == "--help") {
			std::cout << "Flags:" << std::endl

				<< "	-i: Disable images" << std::endl
				<< "	-a [ACCOUNT] : Load specific account" << std::endl
				<< "	-t : Disable text" << std::endl
				<< "\t-b : Disable imgur albums" << std::endl
				<< "\t-nv : Disable videos" << std::endl
				<< "	-dc : Disable single comments" << std::endl
				<< "	-ect : Enable the retrieval of comment threads" << std::endl
				<< "	-l[limit] : Sets the limit of the number of comments, the default being 250 items" << std::endl
				<< "	-rha : Enable reddit - html - archiver output" << std::endl
				<< "	-v / --version : Get version" << std::endl
				<< "	-whl / -whitelist[sub, sub] : whitelists a patricular sub or user with -whl" << std::endl
				<< "	-bl / -blacklist[sub, sub] : blackists a paticular sub or user with -bl" << std::endl
				<< "	-sb/ -sortby [subreddit,title,id or unsorted] : Arranges the media downloaded based on the selected sort" << std::endl
				<< "	-r/-reverse reverses : the list of saved items" << std::endl
				<< "	-uw [user,user] : Enable whitelisting users" << std::endl
				<< "	-ub	[user,user] : Enable blacklisting of users" << std::endl
				<< "\t-bd [domain,domain] : Enable blacklisting of domain names" << std::endl
				<< "\t-bw [domain,domain] : Enable whitelisting of domain names" << std::endl
				<< "	-vb : Enable output of more logs" << std::endl;
			return false;
		}
		else if (arg == "-v" || arg == "-version") {
#if defined(VERSION)
			std::cout << VERSION << std::endl;
#else
			std::cout << "No set version" << std::endl;
#endif
			return false;
		}
		else if (arg == "-l") {
			if (i + 1 >= argc) {
				std::cout << "Secondary argument for -l option not present" << std::endl;
				return false;
			}

			args.limit = atoi(argv[i + 1]);
			i++;
		}
		else if (arg == "-whitelist" || arg == "-whl") {
			if (i + 1 >= argc) {
				std::cout << "Second argument for -whitelist/-whl options not present" << std::endl;
				return false;
			}
			if (std::string comma_check = argv[i + 1]; comma_check.rfind(",") != std::string::npos) {

				boost::split(args.whitelist, argv[i + 1], boost::is_any_of(","));
			}
			else {
				args.whitelist.push_back(argv[i + 1]);
			}
			for (auto& elem : args.whitelist)
				boost::algorithm::to_lower(elem);
			i++;
		}
		else if (arg == "-blacklist" || arg == "-bl") {
			if (i + 1 >= argc) {
				std::cout << "Second argument for -blacklist/-bl options not present" << std::endl;
				return false;
			}
			if (std::string comma_check = argv[i + 1]; comma_check.rfind(",") != std::string::npos) {

				boost::split(args.blacklist, argv[i + 1], boost::algorithm::is_any_of(","));
			}
			else {
				args.blacklist.push_back(argv[i + 1]);
			}
			for (auto& elem : args.whitelist)
				boost::algorithm::to_lower(elem);
			i++;
		}
		else if (arg == "-sb" || arg == "-sortby")
		{
			if (i + 1 >= argc) {
				std::cout << "Second argument for -sb/-sortby options not present" << std::endl;
				return false;
			}
			std::string sort = argv[i + 1];
			boost::algorithm::to_lower(sort);
			if (sort == "subreddit" || sort == "sub")
			{
				args.sort = Subreddit;
			}
			else if (sort == "id") {
				args.sort = ID;
			}
			else if (sort == "title") {
				args.sort = Title;
			}
			else if(sort == "unsorted"){
				args.sort = Unsorted;
			}
			else {
				args.sort = Subreddit;
			}
			i++;
		}
		else if (arg == "-r" || arg == "-reverse") {
			args.reverse = true;
		}
		else if(arg == "-ub") {
			if (i + 1 >= argc) {
				std::cout << "Second argument for -ub options not present" << std::endl;
				return false;
			}
			if (std::string comma_check = argv[i + 1]; comma_check.rfind(",") != std::string::npos) {

				boost::split(args.ublacklist, argv[i + 1], boost::algorithm::is_any_of(","));
			}
			else {
				args.ublacklist.push_back(argv[i + 1]);
			}
			for (auto& elem : args.ublacklist)
				boost::algorithm::to_lower(elem);
			i++;
		} else if(arg == "-uw") {
			if (i + 1 >= argc) {
				std::cout << "Second argument for -uw options not present" << std::endl;
				return false;
			}
			if (std::string comma_check = argv[i + 1]; comma_check.rfind(",") != std::string::npos) {

				boost::split(args.uwhitelist, argv[i + 1], boost::algorithm::is_any_of(","));
			}
			else {
				args.uwhitelist.push_back(argv[i + 1]);
			}
			for (auto& elem : args.uwhitelist)
				boost::algorithm::to_lower(elem);
			i++;
		}
		else if(arg == "-bd") {
			if (i + 1 >= argc) {
				std::cout << "Second argument for -bd options not present" << std::endl;
				return false;
			}
			if (std::string comma_check = argv[i + 1]; comma_check.rfind(",") != std::string::npos) {

				boost::split(args.dblacklist, argv[i + 1], boost::algorithm::is_any_of(","));
			}
			else {
				args.dblacklist.push_back(argv[i + 1]);
			}
			for (auto& elem : args.ublacklist)
				boost::algorithm::to_lower(elem);
			i++;
		} else if(arg == "-wd") {
			if (i + 1 >= argc) {
				std::cout << "Second argument for -wd options not present" << std::endl;
				return false;
			}
			if (std::string comma_check = argv[i + 1]; comma_check.rfind(",") != std::string::npos) {

				boost::split(args.dwhitelist, argv[i + 1], boost::algorithm::is_any_of(","));
			}
			else {
				args.dwhitelist.push_back(argv[i + 1]);
			}
			for (auto& elem : args.dwhitelist)
				boost::algorithm::to_lower(elem);
			i++;
		} else if(arg == "-vb") {
			args.Verbose = true;
		} else if(arg == "-b") {
            args.EnableImgurAlbums = false;
        } else if(arg == "-nv") {
            args.VideosEnabled = false;
        } else if(arg == "-ect") {
            args.EnableCommentThreads = true;
        }
		else {
			std::cerr << "Error, unkown command: " << argv[i] << std::endl;
			std::cout << "Try -h or --help for a list of commands" << std::endl;
			return false;
		}
	}
	return true;
}

State Saver::RetrieveComments(Item* i)
{
	is_mtime_up();
	if (!is_time_up())
	{
		return retrieve_comments(i);
	}
	else {
		State res = obtain_token(true);
		if (res.http_state != 200) {
			return res;
		}
		else {
			return retrieve_comments(i);;
		}
	}
}

State Saver::AccessPosts(std::vector< Item* >& saved)
{
	State s;
	for(int i = 0; i < 1000; i += 100)
	{
		is_mtime_up();
		if (!is_time_up())
		{
			s = get_saved_items(saved, after);
		}
		else {
			State res = obtain_token(true);
			if (res.http_state != 200)
			{
				return res;
			}
			else {
				s = get_saved_items(saved, after);
			}
		}
		if(s.http_state != 200)
			break;
	}
	std::cout << "Total saved items: " << saved.size() << std::endl;
	return s;
}

bool is_in_list(std::vector<std::string> lhs, std::string rhs)
{
    std::vector<std::string>::iterator it = std::find(std::begin(lhs), std::end(lhs), rhs);
    return (it != std::end(lhs));
}
