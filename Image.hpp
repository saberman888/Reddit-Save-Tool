#pragma once

#include "RedditCommon.hpp"
#include "Imgur.hpp"
#include "base.hpp"
#include <string>

namespace RST
{
		class Image : public RedditCommon
		{
		public:
			Image(const nlohmann::json& json);
			Image(const nlohmann::json& json, std::string 
ImgurClientId);
			bool Write(std::filesystem::path dest);
		private:
			std::string ImgurClientId;
			std::string GetImage();
		};
};
