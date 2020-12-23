#include "Imgur.hpp"

namespace ImgurAccess
{ 
    static State ImgurGet(std::string endpoint, std::string ClientId)
	{
		std::string URL = "https://api.imgur.com" + endpoint;
        
        BasicRequest handle;    
        handle.Setup(URL);
		std::string ImgurHeader =
			"Authorization: Client-ID "
			+ ClientId;

		handle.SetHeaders(ImgurHeader);
		handle.SetOpt(CURLOPT_FOLLOWLOCATION, 1L);
		State result = handle.SendRequest();
		handle.Cleanup();
		return result;
	}

	static std::string ParseImage(std::string json)
	{
		std::string data;
		try {
			nlohmann::json root = nlohmann::json::parse(json);
			if (root.contains("data"))
			{
				data = root.at("data").at("link").get <std::string>();
			}
		}
		catch (nlohmann::json::out_of_range&) {
			throw;
		}
		return data;
	}
	static std::vector<std::string> ParseAlbum(std::string json)
	{
		std::vector<std::string> URLs;
		try {
			nlohmann::json root = nlohmann::json::parse(json);
			nlohmann::json images = root.at("data").at("images");
			for (auto& elem : images)
			{
				URLs.push_back(elem.at("link").get<std::string>());
			}
		}
		catch (nlohmann::json::exception&) {
			throw;
		}
		return URLs;
	} 
	
	std::string GetImage(std::string ImageHash, std::string ClientId)
	{
    std::string endpoint = "/3/image/" + GetHash(ImageHash);
		auto result = ImgurGet(endpoint, ClientId);
		if (result.HttpState == 200)
		{
			return ParseImage(result.buffer);
		}
		return std::string();
	}

	std::vector<std::string> GetAlbum(std::string Album, std::string ClientId)
	{
		std::string endpoint = "/3/album/" + GetHash(Album) + "/images";
		std::vector<std::string> Images;
		auto result = ImgurGet(endpoint, ClientId);
		if (result.AllGood())
		{
			Images = ParseAlbum(result.buffer);
		}
		return Images;
	}

  std::vector<std::string> ResolveAlbumURLs(std::string URL, std::string ClientId)
  {
      std::vector<std::string> Images;
      if(IsImgurLink(URL) && !IsDirect(URL))
      {
          if(IsAlbum(URL))
          {
              Images = ImgurAccess::GetAlbum(URL, ClientId);
          }
      }
    return Images;
  }


}
