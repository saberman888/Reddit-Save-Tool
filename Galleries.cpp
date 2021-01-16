#include "Galleries.hpp"

namespace RST
{
	Gallery::Gallery(const nlohmann::json& json)
	{
		Read(json);
	}

	Gallery::Gallery(const nlohmann::json& json, std::string ImgurClientId) : ImgurClientId(ImgurClientId)
	{
		Read(json);
	}

	void Gallery::Read(const nlohmann::json& json, bool ReadDomain)
	{
		using namespace SBJSON;
		RedditCommon::Read(json);
		if (ImgurClientId.empty()) {
			for (auto& image : json.at("gallery_data").at("items"))
			{
				std::string mediaId = GetValue<std::string>(image, "media_id");
				auto mediaMetadata = json.at("media_metadata").at(mediaId);

				std::string imageExtension = splitString(GetValue<std::string>(mediaMetadata, "m"), '/')[1];
				std::string imageURL = "https://i.redd.it/" + mediaId + "." + imageExtension;
				Images.push_back(imageURL);
			}
		}
	}

	bool Gallery::Write(std::filesystem::path dest) {
		auto images = GetImages();
		for (size_t i = 0; i < images.size(); i++) {
			auto image = images[0];
			auto imageData = Download(image);
			if (imageData.AllGood()) {
				auto ContentType = splitString(imageData.ContentType, '/');
				if (ContentType[0] == "image") {
					if (!std::filesystem::exists(dest))
					{
						std::filesystem::create_directories(dest);
					}

					RST::Write(dest / (Id + "." + ContentType[1]), imageData.buffer);
				}
			}
		}
		return true;
	}

	const std::vector<std::string> Gallery::GetImages()
	{
        if (ImgurClientId.empty())
		{
			return Images;
		}
		else {
			return ImgurAccess::ResolveAlbumURLs(URL, ImgurClientId);
		}
	}
};


