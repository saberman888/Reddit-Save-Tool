#include "Saver.hpp"
#include <iostream>



bool RST::Saver::LoadConfig()
{
	char* HOME = std::getenv("HOME");
	std::string config;
	if (fs::exists("settings.json"))
	{
		config = "./settings.json";
	}
#if defined(__unix__)
	else if (fs::exists(std::string(HOME) + "/.config/reddit-saver/settings.json")) {
		config = std::string(HOME) + "/.config/reddit-saver/settings.json";
	}
#endif
	else {
		std::cerr << "Error, couldn't find a settings.json file" << std::endl;
		std::cout << "Generating a template..." << std::endl;
		std::ofstream out;
#if defined(__unix__)
		fs::create_directories(std::string(HOME) + "/.config/reddit-saver");
		out.open(std::string(HOME) + "/.config/reddit-saver/settings.json", std::ios::out);
		std::cout << "Writing to " << HOME << "/.config/reddit-saver/settings.json" << std::endl;
#else
		out.open("settings.json", std::ios::out);
#endif

		nlohmann::json settingsfilling = {
		{"imgur_client_id", nullptr} };

		Json account = {
		{"client_id", "CLIENT_ID_HERE"},
		{"secret", "SECRET_HERE"},
		{"username", "USERNAME_HERE"},
		{"password", "PASSWORD_HERE"},
		{"useragent", "USERAGENT_HERE"} };

		settingsfilling["accounts"].push_back(account);

		out << settingsfilling.dump(4);
		return false;
	}
	return LoadLogins(config);
}

bool RST::Saver::LoadLogins(std::string config)
{
	using namespace SBJSON;
	bool success = false;


	std::fstream input(config, std::ios::in | std::ios::out);
	if (!input.good())
	{
		std::cerr << "Error, failed to open settings.json" << std::endl;
		return success;
	}
	else {
		// Read json from the file
		Json root;
		input >> root;

		// See how many accounts are present
		// If there are more than one, continue and scan for the requested account or the first one
		// depending on the option used in args
		if (root.at("accounts").size() > 1)
		{
			for (auto& elem : root.at("accounts"))
			{
				if (std::string username = elem.at("username").get<std::string>(); args["account"] == username)
				{
					UserAccount.Username = username;
					UserAccount.Password = elem.at("password").get<std::string>();
					UserAccount.ClientId = elem.at("client_id").get<std::string>();
					UserAccount.Secret = elem.at("secret").get<std::string>();
					UserAccount.UserAgent = elem.at("useragent").get<std::string>();
					break;
				}
			}

			// Check if a Imgur Client ID is present
			if (root.contains("imgur_client_id"))
			{
				// if it is, assign it to ImgurClientId
				if (root.contains("imgur_client_id") && !root.at("imgur_client_id").is_null())
				{
					ImgurClientId = root.at("imgur_client_id").get<std::string>();
				}
			}
		}
		else {
			auto elem = root.at("accounts").at(0);
			UserAccount.Username = elem.at("username").get<std::string>();
			UserAccount.Password = elem.at("password").get<std::string>();
			UserAccount.ClientId = elem.at("client_id").get<std::string>();
			UserAccount.Secret = elem.at("secret").get<std::string>();
			UserAccount.UserAgent = elem.at("useragent").get<std::string>();
		}
		success = true;
	}
	return success;
}


State RST::Saver::RetrieveSaved()
{
	std::string Endpoint = "user/"
		+ UserAccount.Username
		+ "/saved/"
		+ "?limit="
		+ std::to_string(1000);

	Endpoint +=
		"&after=" + after;
	return RedditGetRequest(Endpoint);
}


bool RST::Saver::ParseSaved(const std::string& buffer)
{
	using namespace SBJSON;
	try {
		nlohmann::json root = nlohmann::json::parse(buffer);
		// See if there is any after tag
		nlohmann::json data = root.at("data");
		// Make sure after is not null. If it is, it means we've reached the end of the listing
		if (!data.at("after").is_null()) { // store it into after
			after = data.at("after").get<std::string>();
		}
		else {
			after = std::string();
		}
		auto children = data.at("children");
		for (nlohmann::json::const_iterator it = children.begin(); it != children.end(); it++) {
			auto child = (*it).at("data");
			dump(child, "dump.json");
			auto kind = GetValue<std::string>(*it, "kind");

			if (TryGetBool(child, "is_video"))
			{
				assert(kind == "t3");
				if (!RST::contains(args, "--no-video") && !IsBlocked(child)) {
					std::shared_ptr<RedditCommon*> vid = std::make_shared<RedditCommon*>(new Video(child));
					posts.push_back(vid);
				}
			}
			else if (TryGetBool(child, "is_gallery") && !child.at("gallery_data").is_null()) {
				assert(kind == "t3");
				if ((!RST::contains(args, "--no-galleries") && !RST::contains(args, "--no-images")) && !IsBlocked(child)) {
					std::shared_ptr<RedditCommon*> gal = std::make_shared<RedditCommon*>(new Gallery(child));
					posts.push_back(gal);
				}
			}
			else if (TryGetBool(child, "is_self") && child.contains("selftext")) {
				assert(kind == "t3");
				if (!RST::contains(args, "--no-selfposts") && !IsBlocked(child)) {
					std::shared_ptr<RedditCommon*> sp = std::make_shared<RedditCommon*>(new SelfPost(child));
					posts.push_back(sp);
				}
			}
			else if (child.contains("link_url") && !TryGetBool(child, "is_self")) {
				assert(kind == "t1");
				if (!RST::contains(args, "--no-comments")) {
					std::shared_ptr<RedditCommon*> comment = std::make_shared<RedditCommon*>(new Comment(child));
					posts.push_back(comment);
				}
			}
			else {
				assert(kind == "t3");
				if (!RST::contains(args, "--no-images") && !IsBlocked(child)) {
					std::shared_ptr<RedditCommon*> img = std::make_shared<RedditCommon*>(new Image(child));
					posts.push_back(img);
				}
			}
		}
	}
	catch (nlohmann::json::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

RST::Saver::Saver(int argc, char* argv[]) : after(), ImgurClientId()
{

	if (ScanOptions(argc, argv) && LoadConfig()) {
		// If on Windows ,just store everthing in the current working directory under Reddit
		// If under Linux, store stuff in the home directory under /home/$USER/Pictures/Reddit/$REDDITUSERNAME
#if defined(_WIN32) || defined(WIN32)
		MediaPath /= std::string("Reddit/" + UserAccount.Username);
#else
		char* home = std::getenv("HOME");

		MediaPath /=
			std::string(home)
			+ std::string("/Pictures/Reddit/" + UserAccount.Username);

#endif
		std::cout << "Writing to: " << MediaPath.string() << std::endl;

		if (AccessReddit() && GetSaved())
		{
			std::cout << "Gathered a total of: " << posts.size() << " posts" << std::endl;
#if defined(USE_OPENMP)
#pragma omp parallel for
			for (auto& post : posts)
#else
			for (auto& post : posts)
#endif
			{
				try {
					(*post)->Write(MediaPath);
				}
				catch (std::exception& e) {
					std::cerr << "Uh Oh! Something went wrong!" << std::endl;
					std::cerr << e.what() << std::endl;
				}
			}
		}
	}
	else {
		std::cout << "Failed to load account(s)." << std::endl;
	}
}

bool RST::Saver::GetSaved()
{
	do
	{
		auto r = RetrieveSaved();
		if (!ParseSaved(r.buffer))
		{
			return false;
		}
	} while (!after.empty());
	return true;
}

bool RST::Saver::ScanOptions(int argc, char* argv[])
{
	if (argc <= 1)
	{
		std::cout << "Not enough arguments!" << std::endl;
		std::cout << "Flags: " << std::endl;
		std::cout << "\t" << "-v/--version - Print the current build version" << std::endl;
		std::cout << "\t" << "-a/--account [account] - Load a specific reddit account" << std::endl;
		std::cout << "\t" << "--no-text - Filters out any comments and self posts" << std::endl;
		std::cout << "\t" << "--no-selfposts - Filters out any selfposts" << std::endl;
		std::cout << "\t" << "--no-comments - Filters out any comments" << std::endl;
		std::cout << "\t" << "--no-images - Filters out any images" << std::endl;
		std::cout << "\t" << "--only-video - Filters out any posts that aren't videos" << std::endl;
		std::cout << "\t" << "--only-images - Filters out any posts that aren't images" << std::endl;
		std::cout << "\t" << "--domain [domain],[domain2] - Filters out any posts that have said domain linked" << std::endl;
		return false;
	}
	for (int i = 1; i < argc; i++)
	{
		std::string j = argv[i];
		if (j == "-a" || j == "--account")
		{
			if (i + 1 > argc - 1)
			{
				std::cerr << "Error, missing an argument for -a/--account" << std::endl;
				return false;
			}
			else {
				i++;
				args["account"] = std::string(argv[i]);
			}
		}
		else if (j == "--no-images" || j == "--no-selfposts" || j == "--no-comments" || j == "--no-videos") {
			args[j] = "0";
		}
		else if (j == "--no-text") {
			args["--no-comments"] = "0";
			args["--no-selfposts"] = "0";
		}
		else if (j == "--only-video") {
			args["--no-comments"] = "0";
			args["--no-selfposts"] = "0";
			args["--no-images"] = "0";
		}
		else if (j == "--only-images") {
			args["--no-comments"] = "0";
			args["--no-selfposts"] = "0";
			args["--no-video"] = "0";
		}
		else if (j == "--domain" || j == "-D") {
			if (i + 1 > argc - 1)
			{
				std::cerr << "Error, not enough arguments for --domain" << std::endl;
				return false;
			}
			else {
				args["--domain"] = std::string(argv[i + 1]);
				i++;
			}
		}
		else if (j == "-v" || j == "--version") {
			std::cout << "Reddit Save Tool version " << VERSION << std::endl;
		}
		else {
			std::cerr << j << " is not a valid argument." << std::endl;
			std::cout << "Flags: " << std::endl;
			std::cout << "\t" << "-v/--version - Print the current build version" << std::endl;
			std::cout << "\t" << "-a/--account [account] - Load a specific reddit account" << std::endl;
			std::cout << "\t" << "--no-text - Filters out any comments and self posts" << std::endl;
			std::cout << "\t" << "--no-selfposts - Filters out any selfposts" << std::endl;
			std::cout << "\t" << "--no-comments - Filters out any comments" << std::endl;
			std::cout << "\t" << "--no-images - Filters out any images" << std::endl;
			std::cout << "\t" << "--only-video - Filters out any posts that aren't videos" << std::endl;
			std::cout << "\t" << "--only-images - Filters out any posts that aren't images" << std::endl;
			std::cout << "\t" << "--domain [domain],[domain2] - Filters out any posts that have said domain linked" << std::endl;
			return false;
		}
	}
	return true;
}


std::vector<std::string> RST::Saver::GetListOp(std::string option)
{
	std::vector<std::string> elements;
	if (contains(args, option))
	{
		std::string op = args[option];
		elements = splitString(op, ',');
	}
	return elements;
}

bool RST::Saver::IsBlocked(const nlohmann::json& PostData) {
	auto domains = GetListOp("--domain");
	for (auto& arg : args) {
		if (arg.first == "--domain") {
			if (std::any_of(domains.begin(), domains.end(), [&PostData](std::string domain) { return SBJSON::GetValue<std::string>(PostData, "domain") == domain; }))
				return true;
		}
	}
	return false;
}


