#include "Image.hpp"

namespace RST
{
	Image::Image(const nlohmann::json& json)
	{
		RedditCommon::Read(json);
	}

	Image::Image(const nlohmann::json& json, std::string
		ImgurClientId) : ImgurClientId(ImgurClientId)
	{
		RedditCommon::Read(json);
	}

	std::string Image::GetImage() {
		if (!ImgurClientId.empty()) {
			return ImgurAccess::GetImage(URL,
				ImgurClientId);
		}
		else {
			return URL;
		}
	}


	bool Image::Write(std::filesystem::path dest) {
		auto image = Download(URL);
		if (!image.AllGood()) {
			std::cerr << "Error, failed to get image " << URL << std::endl;
			std::cerr << image.HttpState << " " << image.Message << std::endl;

			return false;
		}
		auto ContentType = splitString(image.ContentType, '/');

		if (ContentType[0] == "image") {
			RST::Write(dest / (Id + "." + ContentType[1]), image.buffer);
		}
		return true;
	}
};
