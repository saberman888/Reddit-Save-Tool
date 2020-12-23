#include "Galleries.hpp"

namespace RST
{
	Gallery::Gallery(const std::string& json)
	{
		Read(json);
	}

	Gallery::Gallery(const std::string& json, std::string ImgurClientId) : ImgurClientId(ImgurClientId)
	{
		Read(json);
	}

	void Gallery::Read(const std::string& json)
	{
		using namespace SBJSON;
		RedditCommon::Read(json);
		if(!ImgurClientId.empty()){
			nlohmann::json root = nlohmann::json::parse(json);
			for(auto& image : root.at("gallery_data").at("item"))
			{
				std::string mediaId = GetValue<std::string>(image, "media_id");
				auto mediaMetadata = image.at("media_metadata").at(mediaId);
				
				std::string imageExtension = splitString(GetValue<std::string>(mediaMetadata, "m"), '/')[1];
				std::string imageURL = "https://i.reddit.it/" + mediaId + "." + imageExtension;
				Images.push_back(imageURL);
				

			}
		}
	}
	
	bool Gallery::Write(std::filesystem::path dest){
		auto images = GetImages();
		for(size_t i = 0; i < images.size(); i++){
			auto image = images[0];
			auto imageData = Download(image);
			if(imageData.AllGood()){
				RST::Write(dest / (Id + RST::GetImageExtension(imageData)), imageData.buffer);
			}
		}
		return true;
	}

	const std::vector<std::string> Gallery::GetImages()
	{
		if(!ImgurClientId.empty())
		{
			return Images;
		} else {
			return ImgurAccess::ResolveAlbumURLs(URL, ImgurClientId);
		}
	}
};


