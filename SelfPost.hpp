#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include "sbjson.hpp"
#include "BasicRequest.hpp"
#include "RedditCommon.hpp"

namespace RST
{
	class SelfPost : public RedditCommon
	{
	public:
		SelfPost(const nlohmann::json& json);
		bool Write(std::filesystem::path destination);
	private:
		std::string Text;
		void Read(const nlohmann::json& json, bool ReadDomain = true);
	};
};
