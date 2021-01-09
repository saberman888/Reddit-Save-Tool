#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include "RedditCommon.hpp"
#include "BasicRequest.hpp"

namespace RST
{
	class Comment : public RedditCommon
	{
	public:
		Comment(const nlohmann::json& json);
		bool Write(std::filesystem::path destination);
	private:
		std::string Text;
		void Read(const nlohmann::json& json, bool ReadDomain = true);
	};
};
