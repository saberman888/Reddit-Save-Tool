#include "Saver.hpp"
#include <iostream>



bool RST::Saver::LoadConfig()
{
    char* HOME = std::getenv("HOME");
    std::string config;
    if(fs::exists("settings.json"))
    {
        config = "./settings.json";
    }
#if defined(__unix__)
    else if(fs::exists(std::string(HOME) +"/.config/reddit-saver/settings.json")) {
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

        nlohmann::json settingsfilling ={
        {"imgur_client_id", nullptr}};

        Json account = {
        {"client_id", "CLIENT_ID_HERE"},
        {"secret", "SECRET_HERE"},
        {"username", "USERNAME_HERE"},
        {"password", "PASSWORD_HERE"},
        {"useragent", "USERAGENT_HERE"}};

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
				if(root.contains("imgur_client_id") && !root.at("imgur_client_id").is_null())
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
			// If there is an after tag, it is pretty much a given
			// that a listing is present too, so add that listing into posts
			for (auto& child : data.at("children"))
			{
        RedditObject r;
        r.Read(child);
        posts.push_back(r);
			}
	} catch(nlohmann::json::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

RST::Saver::Saver(int argc, char* argv[]) : after(), ImgurClientId()
{

  if(ScanOptions(argc, argv) && LoadConfig()){
    // If on Windows ,just store everthing in the current working directory under Reddit
    // If under Linux, store stuff in the home directory under /home/$USER/Pictures/Reddit/$REDDITUSERNAME
#if defined(_WIN32) || defined(WIN32)
    MediaPath /= std::string("./Reddit/" + UserAccount.Username);
#else
    char* home = std::getenv("HOME");

    MediaPath /=
      std::string(home)
      +std::string("/Pictures/Reddit/"+UserAccount.Username);

#endif
    std::cout << "Writing to: " << MediaPath.string() << std::endl;

    if(AccessReddit() && GetSaved())
    {
      FilterPosts();
      std::cout << "Gathered a total of: " << posts.size() << " posts" << std::endl;
#if defined(USE_OPENMP)
      #pragma omp parallel for
      for(auto& post : posts)
#else
      for(auto& post : posts)
#endif
      {
        try {
          WritePost(post);
        }catch(std::exception& e) {
          std::cerr << "Uh Oh! Something went wrong!" << std::endl;
          std::cerr << e.what() << std::endl;
        }
      }
    }
  } else {
    std::cout << "Failed to load account(s)." << std::endl;
  }
}

bool RST::Saver::GetSaved()
{
	do
	{
		auto r = RetrieveSaved();
		if(!ParseSaved(r.buffer))
		{
			return false;
		}
	} while (!after.empty());
	return true;
}

State RST::Saver::Download(std::string URL)
{
	BasicRequest handle;
	handle.Setup(URL);
	handle.SetOpt(CURLOPT_FOLLOWLOCATION, 1L);
	State result = handle.SendRequest();
	handle.Cleanup();
	return result;
}

bool RST::Saver::DownloadImage(RedditObject& post, fs::path destination, std::string filename)
{
  State response = Download(post.URL);
  if(response.AllGood())
  {
    // Make the sure the link is an actual image
    auto ContentType = splitString(splitString(response.ContentType, ';')[0], '/');
    if(ContentType[0] == "image"){
      std::string extension = "." + ContentType[1];
      if(filename.empty())
          filename = post.Id + extension;
      else
          filename += extension;

      post.Write(destination, filename, response.buffer);
    }
  } else {
      std::cerr << "Error: " << response.HttpState << " " << response.Message << std::endl;
    return false;
  }
  return true;
}





bool RST::Saver::WritePost(RedditObject& post)
{
	if(post.kind == VIDEO){
        auto video = Download(post.URL);
        if (!video.AllGood())
        {
            std::cerr << "Error: Failed to get video from " << post.Id << std::endl;
            std::cerr << "Error: " << video.HttpState << " " << video.Message << std::endl;
            return false;
        }
        post.Write(MediaPath, "video.mp4", video.buffer);

        auto dash = Download(post.DASHPlaylistFile);
        if (!dash.AllGood())
        {
            std::cerr << "Error: Failed to get DASHPlaylist.mpd from " << post.Id << std::endl;
            std::cerr << "Error: " << dash.HttpState << " " << dash.Message << std::endl;
            return false;
        }
        // See if the video has any audio at all
        if (std::regex_search(dash.buffer, std::regex("<BaseURL>(DASH_)audio(.mp4)</BaseURL>")))
        {
            auto audio  = Download(post.GetAudioUrl());
            if (!audio.AllGood())
            {
                std::cerr << "Error: Failed to get audio from " << post.Id << std::endl;
                std::cerr << "Error: " << audio.HttpState << " " << audio.Message << std::endl;
                return false;
            }
            post.Write(MediaPath, "audio.mp4", audio.buffer);
            // and finally mux the audio and video together
            post.MuxVideo(MediaPath.string(), MediaPath.string());
        }
        // If it doesn't, just rename the existing mp4 we downloaded
        else {
            fs::rename(MediaPath / "video.mp4", MediaPath / (post.Id + ".mp4"));
        }

	} else if(post.kind == LINKPOST) {
        // If there is any imgur links, resolve them so we can have the links directly to the images
        if(!ImgurClientId.empty())
            post.ResolveImgurLinks(ImgurClientId);
        
		if(post.Gallery.IsGallery)
        {
            fs::path savepath = MediaPath / post.title;
            for(size_t i = 0; i < post.Gallery.Images.size(); i++)
            {
                post.URL = post.Gallery.Images[i];

                std::string filename = std::to_string(i) + post.Id;

                bool result = DownloadImage(post, savepath, filename);
                if(!result){
                    std::cerr << "Error to download " << post.URL << std::endl;
                    return result;
                }
                std::cout << "Wrote Gallery: " << post.title << ": " << i << " out of " << post.Gallery.Images.size()-1 << std::endl;
            }
        } else {
           bool result = DownloadImage(post, MediaPath);
           if(!result){
               std::cerr << "Error failed to download " << post.URL << " from " << post.Id << std::endl;
               return result;
           }
        }
	} else if(post.kind == SELFPOST || post.kind == COMMENT){
		post.WriteText(MediaPath);
	}
	return true;
}


bool RST::Saver::ScanOptions(int argc, char* argv[])
{
  if(argc <= 1)
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
	for(int i = 1; i < argc; i++)
	{
		std::string j = argv[i];
    if(j == "-a" || j == "--account")
    {
     if(i+1 > argc-1 )
     {
       std::cerr << "Error, missing an argument for -a/--account" << std::endl;
       return false;
     } else {
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
    } else if(j == "--domain" || j == "-D") {
      if(i+1 > argc-1)
      {
        std::cerr << "Error, not enough arguments for --domain" << std::endl;
        return false;
      } else {
        args["--domain"] = std::string(argv[i+1]);
        i++;
      }
    } else if(j == "-v" || j == "--version") {
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
    if(contains(args,option))
    {
        std::string op = args[option];
        elements = splitString(op, ',');
    }
    return elements;
}



void RST::Saver::FilterPosts()
{
    for (auto& arg : args)
    {
        if (arg.first == "--domain")
        {
            auto domains = GetListOp("--domain");

            auto it = std::remove_if(posts.begin(), posts.end(), [&domains](RedditObject& post) {
                return std::any_of(domains.begin(), domains.end(), [&post](std::string domain) { return post.domain == domain; });
                });
            posts.erase(it, posts.end());
        }
        else if (arg.first == "--no-comments") {
            std::erase_if(posts, [](RedditObject& post) { return post.kind == COMMENT; });
        }
        else if (arg.first == "--no-images") {
            std::erase_if(posts, [](RedditObject& post) { return post.kind == LINKPOST; });
        }
        else if (arg.first == "--no-selfposts") {
            std::erase_if(posts, [](RedditObject& post) { return post.kind == SELFPOST; });
        }
        else if (arg.first == "--no-video") {
            std::erase_if(posts, [](RedditObject& post) { return post.kind == VIDEO; });
        }
    }
}
