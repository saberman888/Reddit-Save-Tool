#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include "sbjson.hpp"
#include "Imgur.hpp"
#include "Image.hpp"
#include "RedditCommon.hpp"
#include "nlohmann/json.hpp"

namespace RST
{
	class Gallery : public RedditCommon
	{
		public:
			// Initializes and then passes a json string into Read(const std::string& json)
			Gallery(const std::string& json);
			Gallery(const std::string& json, std::string ImgurClientId);

			const std::vector<std::string> GetImages();
			bool Write(std::filesystem::path dest);
		private:
			// Reads json provided in json and initializes variables from it
			void Read(const std::string& json);
			std::string ImgurClientId;
		
			// Where the images will be stored
			std::vector<std::string> Images;

	};
};

