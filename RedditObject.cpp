#include "RedditObject.hpp"
namespace RST {
  std::string RedditObject::GetAudioUrl()
  {
    return URL + "/audio?source=fallback";
  }

  std::string RedditObject::GetVideoUrl()
  {
    return URL + "/DASH_" + std::to_string(Video.height)+".mp4";
  }

  void RedditObject::MuxVideo(std::string source, std::string dest)
  {
      std::string filename = Id + ".mkv";
      
      std::string ffmpegCommand = "ffmpeg -y -i ";
      ffmpegCommand.append(source); ffmpegCommand.append("/video.mp4 -i ");
      ffmpegCommand.append(source); ffmpegCommand.append("/audio.mp4 -c copy ");
      ffmpegCommand.append(dest); ffmpegCommand.append("/" + filename);
      std::system(ffmpegCommand.c_str());
  }

  void RedditObject::Write(fs::path filepath, std::string filename, const std::string& buffer)
  {
    fs::create_directories(filepath);
    fs::path fullpath = filepath / filename;
    std::fstream out(fullpath.string(), std::ios::out | std::ios::binary);
    out << buffer;
  }

  void RedditObject::WriteText(fs::path filepath)
  {
    fs::create_directories(filepath);
    fs::path fullpath = filepath / std::string(Id + ".txt");
    std::fstream out(fullpath.string(), std::ios::out);
    out << "On Post: " << title << std::endl;
    out << "Author: " << author << std::endl;
    out << "Date: " << BasicRequest::UTCToString(created_utc, "%Y-%m-%d @ %H:%M %p") << std::endl;
    out << "ID: " << Id << std::endl;
    out << "Permalink: " << permalink << std::endl;
    out << text << std::endl;
  }


  void RedditObject::Read(nlohmann::json& child)
  {
      using namespace SBJSON;
      std::string tempKind = get_string(child, "kind");
      auto data = child.at("data");
      if(tempKind == "t3")
      {
          // Check if selftext exists, because if it doesn't exist. It means the post got deleted before it got archived.
          if(DoesExist(data, "selftext") && get_bool(data, "is_self")){
              kind = SELFPOST;
              text = get_string(data, "selftext");
          } else if(DoesExist(data, "is_video") && get_bool(data, "is_video")){
              kind = VIDEO;
              
              // Get the height only because when we download the video file
              // it usually titled as DASH_{height} e.g DASH_1080.mp4
              auto redditvideo = data.at("media").at("reddit_video");
              Video.height = get_int(redditvideo, "height");
              Video.IsGif = get_bool(redditvideo, "is_gif");
          } else {
              kind = LINKPOST;
              if(data.contains("is_gallery") && get_bool(data, "is_gallery"))
              {
                  Gallery.IsGallery = true;
                  for(auto& elem : data.at("gallery_data").at("items"))
                  {
                      
                      std::string id = get_string(elem, "media_id");
                      auto metadata = data.at("media_metadata").at(id);
                      std::string extension  = splitString(get_string(metadata, "m"), '/')[1];

                      std::string imageURL = "https://i.redd.it/" + id + "." + extension;
                      Gallery.Images.push_back(imageURL);
                  }
              }
         }
        URL = get_string(data, "url");
        title = get_string(data, "title");
        domain = get_string(data, "domain");

      } else if(tempKind == "t1")
      {
          kind = COMMENT;
          text = get_string(data, "body");
          title = get_string(data, "link_title");
          URL = get_string(data, "link_url");
      }
      Id = get_string(data, "id");
      author = get_string(data, "author");
      permalink = get_string(data, "permalink");
      created_utc = get_long(data, "created_utc");
  }

  void RedditObject::ResolveImgurLinks(std::string ClientId)
  {
      if(ImgurAccess::IsImgurLink(URL) && !ImgurAccess::IsDirect(URL))
      {
          if(ImgurAccess::IsAlbum(URL))
          {
              Gallery.IsGallery = true;
              Gallery.Images = ImgurAccess::GetAlbum(URL, ClientId);
          } else {
              std::string tempURL = ImgurAccess::GetImage(URL, ClientId);
              URL = tempURL;
          }
      }
  }
};
