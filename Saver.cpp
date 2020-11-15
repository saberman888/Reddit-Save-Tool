#include "Saver.hpp"
#include <iostream>



bool RST::Saver::LoadLogins()
{
	using namespace SBJSON;
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
				//ImgurHandle->ClientId = root.at("imgur_client_id").get<std::string>();
				if(root.contains("imgur_client_id"))
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
                RedditObject r;
                r.Read(child);
                
                switch(r.kind)
                {
                    case SELFPOST:
                        if(!contains(args, "--no-selfposts"))
                            posts.push_back(r);
                        break;
                    case VIDEO:
                        if(!contains(args, "--no-videos"))
                            posts.push_back(r);
                        break;
                    case COMMENT:
                        if(!contains(args, "--no-comments"))
                            posts.push_back(r);
                        break;
                    case LINKPOST:
                        if(!contains(args, "--no-images") || (ImgurAccess::IsImgurLink(r.URL) && !ImgurClientId.empty()))
                            posts.push_back(r);
                        break;
                    case UNKNOWN:
                        break;
                    default:
                        break;
                }
			}
	} catch(nlohmann::json::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

RST::Saver::Saver(int argc, char* argv[]) : after(), ImgurClientId()
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
      std::cout << "Gathered a total of: " << posts.size() << " posts" << std::endl;
      FilterPosts();
      #pragma omp parallel for
      for(auto& post : posts)
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
    std::cout << "Failed to load account." << std::endl;
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
                std::cout << "Wrote Gallery " << post.title << ": " << i << " out of " << post.Gallery.Images.size()-1 << std::endl;
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
	bool success = false;
  if(argc <= 1)
  {
    std::cout << "Not enough arguments!" << std::endl;
  }

  //std::string filterArgs[] = {"--blacklist"}

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
    } else if(j == "--domain" || j == "-D") {
        args["--domain"] = std::string(argv[i+1]);
        i++;
    //} else if(std::any_of()) {
      
    }
    else {
      std::cerr << j << " is not a valid argument." << std::endl;
      success = false;
    }
  }
  return success;
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
    if(contains(args,"--domain"))
    {
        auto domains = GetListOp("--domain");
        std::vector<RedditObject> tempPosts;
        tempPosts.reserve(posts.size());
        
        std::copy_if(posts.begin(), posts.end(), std::back_inserter(tempPosts), [&domains](RedditObject& elem){ return (std::find(domains.begin(), domains.end(), elem.domain) != domains.end());});
        tempPosts.shrink_to_fit();
        posts = tempPosts;
    } /*else if(contains(args, "--blacklist")){
      
    } else if(contains(args, "--whitelist")){
    }*/
}
