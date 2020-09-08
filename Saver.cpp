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
		nlohmann::json settingsfilling ={
			{"imgur_client_id", "IMGUR_CLIENT_ID_HERE"}
		};

		Json account = {
			{"client_id", "CLIENT_ID_HERE"},
			{"secret", "SECRET_HERE"},
			{"username", "USERNAME_HERE"},
			{"password", "PASSWORD_HERE"},
			{"useragent", "USERAGENT_HERE"}};

			settingsfilling["accounts"].push_back(account);

		input << settingsfilling.dump(4);
		return success;
	}
	else {
		// Read json from the file
		Json root;
		input >> root;

		// See how many accounts are present
		// If there are more than one, continue and scan for the requested account or the first one
		// depending on the option used in args
		if (root.at("accounts").size() > 0)
		{
			for (auto& elem : root.at("accounts"))
			{
				if (std::string username = elem.at("username").get<std::string>(); args.username == username)
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
				// if it is, assign it to ImgurHandle.ClientId
				ImgurHandle.ClientId = root.at("imgur_client_id").get<std::string>();
			}
			success = true;
		}
	}
	return success;
}


void Saver::RetrieveSaved()
{
	std::string endpoint = "user/"
		+ UserAccount.Username
		+ "/saved/"
		+ "?limit="
		+ std::to_string(100);
		if (!after.empty())
		{
			endpoint +=
				"&after=" + after;
		}
		RedditGetRequest(endpoint);
}


bool Saver::ParseSaved()
{
	try {
		 	nlohmann::json root = nlohmann::json::parse(Response.buffer);
			// See if there is any after tag
			nlohmann::json data = root.at("data");
				// Make sure after is not null
			if (!data.at("after").is_null()) { // store it into after
				after = data.at("after").get<std::string>();
			}
			else {
				after = std::string();
			}
			// If there is an after tag, it is pretty much a given
			// that a listing is present too, so add that listing into content
			int size = data.at("children").size();
			for (auto& child : data.at("children"))
			{
				if (child.at("kind").get<std::string>() == "t3")
				{
					auto data = child.at("data");
					RedditObject post;
					post.id = data.at("id").get<std::string>(); 
					post.url = data.at("url").get<std::string>();
					post.is_video = data.at("is_video").get<bool>();
					post.is_self = data.at("is_self").get<bool>();
					content.push_back(post);
				}
			}
	} catch(nlohmann::json::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

Saver::Saver() : after()
{
#if defined(_WIN32) || defined(WIN32)
	//const char* HomeDirectory = "%USERPROFILE%";
	std::string home = ".";
#else
	const char* HomeDirectory = "HOME";
	std::string home = std::getenv(HomeDirectory);
#endif
	
	MediaPath =  home + "/Reddit/" + UserAccount.Username;
}

bool Saver::GetSaved()
{
	RetrieveSaved();
	if (!Response.AllGood())
	{
		std::cerr << "Uh oh! Something went wrong!: I failed to get saved posts from oauth.reddit.com!" << std::endl;
		return false;
	}
	else {
		if (!ParseSaved()) {
			std::cerr << "Uh oh! Something went wrong!: I failed to parse saved posts!" << std::endl;
			return false;
		}
	}
	return true;
}

bool Saver::ScanArgs(int argc, char* argv[])
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

				//boost::split(args.whitelist, argv[i + 1], boost::is_any_of(","));
				args.whitelist = splitString(std::string(argv[i + 1]),',');
			}
			else {
				args.whitelist.push_back(argv[i + 1]);
			}
			for (auto& elem : args.whitelist)
				elem = ToLower(elem);
			i++;
		}
		else if (arg == "-blacklist" || arg == "-bl") {
			if (i + 1 >= argc) {
				std::cout << "Second argument for -blacklist/-bl options not present" << std::endl;
				return false;
			}
			if (std::string comma_check = argv[i + 1]; comma_check.rfind(",") != std::string::npos) {

				//boost::split(args.blacklist, argv[i + 1], boost::algorithm::is_any_of(","));
				args.blacklist = splitString(std::string(argv[i + 1]), ',');
			}
			else {
				args.blacklist.push_back(argv[i + 1]);
			}
			for (auto& elem : args.whitelist)
				elem = ToLower(elem);
			i++;
		}
		else if (arg == "-sb" || arg == "-sortby")
		{
			if (i + 1 >= argc) {
				std::cout << "Second argument for -sb/-sortby options not present" << std::endl;
				return false;
			}
			std::string sort = ToLower(std::string(argv[i + 1]));
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

				//boost::split(args.ublacklist, argv[i + 1], boost::algorithm::is_any_of(","));
			}
			else {
				args.ublacklist.push_back(argv[i + 1]);
			}
			for (auto& elem : args.ublacklist)
				//boost::algorithm::to_lower(elem);
			i++;
		} else if(arg == "-uw") {
			if (i + 1 >= argc) {
				std::cout << "Second argument for -uw options not present" << std::endl;
				return false;
			}
			if (std::string comma_check = argv[i + 1]; comma_check.rfind(",") != std::string::npos) {

				//boost::split(args.uwhitelist, argv[i + 1], boost::algorithm::is_any_of(","));
			}
			else {
				args.uwhitelist.push_back(argv[i + 1]);
			}
			for (auto& elem : args.uwhitelist)
				//boost::algorithm::to_lower(elem);
			i++;
		}
		else if(arg == "-bd") {
			if (i + 1 >= argc) {
				std::cout << "Second argument for -bd options not present" << std::endl;
				return false;
			}
			if (std::string comma_check = argv[i + 1]; comma_check.rfind(",") != std::string::npos) {

				//boost::split(args.dblacklist, argv[i + 1], boost::algorithm::is_any_of(","));
			}
			else {
				args.dblacklist.push_back(argv[i + 1]);
			}
			for (auto& elem : args.ublacklist)
				//boost::algorithm::to_lower(elem);
			i++;
		} else if(arg == "-wd") {
			if (i + 1 >= argc) {
				std::cout << "Second argument for -wd options not present" << std::endl;
				return false;
			}
			if (std::string comma_check = argv[i + 1]; comma_check.rfind(",") != std::string::npos) {

				//boost::split(args.dwhitelist, argv[i + 1], boost::algorithm::is_any_of(","));
			}
			else {
				args.dwhitelist.push_back(argv[i + 1]);
			}
			for (auto& elem : args.dwhitelist)
				//boost::algorithm::to_lower(elem);
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


void Saver::Download(std::string URL)
{
	Setup(URL);
	SetOpt(CURLOPT_FOLLOWLOCATION, 1L);
	SendRequest();
	Cleanup();
}

void Saver::Write(fs::path filepath, std::string filename)
{
	fs::path fullpath = this->MediaPath / filepath;
	fs::create_directories(fullpath);

	std::fstream out(fullpath / filename, std::ios::out | std::ios::binary);
	out << Response.buffer;
}

bool Saver::WriteContent(RedditObject post)
{
	if(post.is_video){
		fs::path TempPath = MediaPath / std::string(UserAccount.Username + "/tmp/");
		Download(post.GetAudioUrl());
		if (!Response.AllGood()) {
			return false;
		}
		else {
			
			Write(TempPath, "audio.mp4");
		}

		Download(post.GetVideoUrl());
		if (!Response.AllGood()) {
			return false;
		}
		else {
			Write(TempPath, "video.mp4");
		}

		std::string ffmpegCommand = 
			"ffmpeg -i " 
			+ TempPath.string() 
			+ "/video.mp4 -i "
			+ TempPath.string() +
			"/audio.mp4 -c copy " 
			+ TempPath.string()
			+ "/"
			+post.id + ".mkv";
		std::system(ffmpegCommand.c_str());
	} else if(!post.is_self) {
		Download(post.url);
		// Check if everything went well and make sure it is an image
		if (std::string image = Response.ContentType.substr(0,6);  Response.AllGood() && image == "image/") {
			std::string extension = "." + splitString(Response.ContentType, ';')[0].substr(6);
			Write(UserAccount.Username, post.id + extension);
			std::cout << "Writing Image: " << post.url << std::endl;
		}
	}
}


bool Saver::IsAVideo(Json Post)
{
	return (Post["is_video"].get<bool>() && (Post["url"].get<std::string>()).rfind("https://v.redd.it", 0) != std::string::npos);
}

bool Saver::IsImage(std::string link)
{
	std::array<std::string, 9> urlsnext = { "i.imgur.com", "i.redd.it", ".jpeg", ".bmp", ".png", ".gif", ".jpg", ".tiff", ".webp" };
	for (std::string elem : urlsnext)
		if (link.rfind(elem) != std::string::npos)
			return true;
	return false;
}
