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
		if (root.at("accounts").size() > 1)
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


State Saver::RetrieveSaved()
{
	std::string Endpoint = "user/"
		+ UserAccount.Username
		+ "/saved/"
		+ "?limit="
		+ std::to_string(1000);

		if (!after.empty())
		{
			Endpoint +=
				"&after=" + after;
		}
		return RedditGetRequest(Endpoint);
}


bool Saver::ParseSaved(const std::string& buffer)
{
	try {
		 	nlohmann::json root = nlohmann::json::parse(buffer);
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
			for (auto& child : data.at("children"))
			{
				if (child.at("kind").get<std::string>() == "t3")
				{
					RedditObject post;
					// Check if the child is an image, video or self post
					// assign it the according data, url and id
					if(get_bool(child, "is_self"))
					{
						//TODO: Implement getting self text
					} else if(get_bool(child, "is_video"))
					{
						auto redditvideo = child.at("media").at("reddit_video");
						if(!get_bool(redditvideo, "is_gif"))
						{
							post.VideoInfo.height = get_int(redditvideo, "height");
							post.VideoInfo.IsGif = false;
						} else {
							// TODO: Implement  getting gifs
							continue;
						}
					} else {
						// Since child is an image assign kind, IMAGE
						post.kind = IMAGE;
					}
					post.URL = get_string(child, "url");
					post.Id = get_string(child, "id");
					posts.push_back(post);
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
	std::string home = ".";
#if defined(_WIN32) || defined(WIN32)
	//const char* HomeDirectory = "%USERPROFILE%";
#else
	char* user = std::getenv("USER");
    
    MediaPath /= std::string(
        "/home/"
        +std::string(user)
        +"/Reddit/"
	+UserAccount.Username);
        
#endif

	MediaPath /=  std::string(home + "/Reddit/" + UserAccount.Username);
}

bool Saver::GetSaved()
{
	for (int i = 0; i < 10; i++)
	{
		auto r = RetrieveSaved();
		if(!ParseSaved(r.buffer))
		{
			return false;
		}
	}
	return true;
}
/*
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
			for (auto& elem : args.ublacklist){
				std::cout << elem << std::endl;
				//boost::algorithm::to_lower(elem);
				//i++;
			}
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
				std::cout << elem;
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
        }
		else {
			std::cerr << "Error, unkown command: " << argv[i] << std::endl;
			std::cout << "Try -h or --help for a list of commands" << std::endl;
			return false;
		}
	}
	return true;
}
*/

State Saver::Download(std::string URL)
{
	BasicRequest handle;
	handle.Setup(URL);
	handle.SetOpt(CURLOPT_FOLLOWLOCATION, 1L);
	State result = handle.SendRequest();
	handle.Cleanup();
	return result;
}


bool Saver::WriteContent(RedditObject post)
{
	if(post.kind == VIDEO){
		// Download audio and video
		// if running on linux use /tmp to temporary store audio and video
		fs::path temp = "/tmp";
		auto result = Download(post.GetAudioUrl());
		if(!result.AllGood())
		{
			std::cerr << "Warning: Failed to get audio from " << post.Id << std::endl;
			std::cerr << "Error: " << result.HttpState<< " " << result.Message << std::endl;
			return false;
		}
		post.Write(temp, "audio.mp4", result.buffer);
		result = Download(post.GetVideoUrl());
		if(!result.AllGood())
		{
			std::cerr << "Warning: Failed to get video from " << post.Id << std::endl;
			std::cerr << "Error: " << result.HttpState << " " << result.Message << std::endl;
			return false;
		}
		post.Write(temp, "video.mp4", result.buffer);
		// and finally mux the audio and video together
		post.MuxVideo(temp.string(), MediaPath.string());
		
	} else if(post.kind == IMAGE) {
		State resp = Download(post.URL);
		// Check if everything went well and make sure it is an image
		if (std::string image = resp.ContentType.substr(0,6);  resp.AllGood() && image == "image/") {
			std::string extension = "." + splitString(resp.ContentType, ';')[0].substr(6);
			post.Write(MediaPath, post.Id + extension, resp.buffer);
			std::cout << "Wrote Image: " << post.URL << std::endl;
		}
	}
	return true;
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
