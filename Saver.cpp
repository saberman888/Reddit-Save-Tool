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

		Endpoint +=
			"&after=" + after;
		return RedditGetRequest(Endpoint);
}


bool Saver::ParseSaved(const std::string& buffer)
{
	try {
#ifndef NDEBUG
    dump(buffer, "buffer.json");
#endif
		 	nlohmann::json root = nlohmann::json::parse(buffer);
#ifndef NDEBUG
      dump(root, "root.json");
#endif
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
			// that a listing is present too, so add that listing into content
			for (auto& child : data.at("children"))
			{
#ifndef NDEBUG
        dump(child, "child.json");
#endif

				//std::fstream out("data.json", std::ios::out);
				//out << child.dump(4);
				RedditObject post;
				auto data = child.at("data");
        // Check if child is a post or t3
				if (child.at("kind").get<std::string>() == "t3")
				{
					// Check if the following conditions are true:
          // 1. is_self is true or if the post is indeed a self post
          // 2. Make sure the the program wasn't called to skip self posts
          // 3. Check if selftext exists, because if it doesn't exist. It means the post got deleted before it got archived.
          if (get_bool(data, "is_self") && !find(args, "--no-selfposts") && DoesExist(data, "selftext"))
					{
						post.text = get_string(data, "selftext");
					}
          // Check if the post is a video and if the program wasn't called to skip videos
					else if (get_bool(data, "is_video") && !find(args, "--no-video"))
					{
            // Get the height only because when we download the video file
            // it usually titled as DASH_{height} e.g DASH_1080.mp4
            auto redditvideo = data.at("media").at("reddit_video");
            post.VideoInfo.height = get_int(redditvideo, "height");
            post.VideoInfo.IsGif = get_bool(redditvideo, "is_gif");

            if (post.VideoInfo.IsGif)
            {
              // TODO: Implement getting GIFs
              continue;
            }
            post.kind = VIDEO;

					}
					else {
						if (find(args, "--no-images"))
							continue;

						// Since child is an image assign kind, IMAGE
						post.kind = LINKPOST;
					}
					post.URL = get_string(data, "url");
				}
				else if(child.at("kind").get<std::string>() == "t1" && !find(args, "--no-comments")) {
					post.kind = COMMENT;
					post.text = get_string(data, "body");
					post.URL = get_string(data, "link_url");
				}
				post.Id = get_string(data, "id");
				posts.push_back(post);
			}
	} catch(nlohmann::json::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

Saver::Saver(int argc, char* argv[]) : after()
{

  if(ScanOptions(argc, argv) && LoadLogins()){
    // If on Windows ,just store everthing in the current working directory under Reddit
    // If under Linux, store stuff in the home directory under /home/$USER/Pictures/Reddit
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
      for(auto& post : posts)
      {
        try {
          WriteContent(post);
        }catch(std::exception& e) {
          std::cerr << "Uh Oh! Something went wrong!" << std::endl;
          std::cerr << e.what() << std::endl;
        }
      }
    }
  } else {
    std::cout << "Failed to load account." << std::endl;
  }
}

bool Saver::GetSaved()
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

	} else if(post.kind == LINKPOST) {
		State resp = Download(post.URL);
		// Check if everything went well and make sure it is an image
		if (std::string image = resp.ContentType.substr(0,6);  resp.AllGood() && image == "image/") {
			std::string extension = "." + splitString(resp.ContentType, ';')[0].substr(6);
			post.Write(MediaPath, post.Id + extension, resp.buffer);
			std::cout << "Wrote Image: " << post.URL << std::endl;
		}
	} else if(post.kind == SELFPOST || post.kind == COMMENT) {
   post.WriteText(MediaPath);
  }
	return true;
}


bool Saver::ScanOptions(int argc, char* argv[])
{
	bool success = false;
  if(argc <= 1)
  {
    std::cout << "Not enough arguments!" << std::endl;
  }

	for(int i = 1; i < argc; i++)
	{
		std::string j = argv[i];
    if(j == "-a" || j == "--account")
    {
      i++;
      args["account"] = std::string(argv[i]);
      success = true;
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
    else {
      std::cerr << j << " is not a valid argument." << std::endl;
      success = false;
    }
  }
  return success;
}
