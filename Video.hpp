#pragma once

#include <string>
#include "nlohmann/json.hpp"
#include "base.hpp"
#include "RedditCommon.hpp"
#include <filesystem>
#include <exception>
#include <regex>

namespace RST
{
	class Video : public RedditCommon
	{
	public:
		Video(const nlohmann::json& json);
		bool Write(std::filesystem::path destination);
	private:
		std::string DashPlaylistUrl, AudioURL;
		bool HasAudio;

		bool CheckForAudio();
		void Mux(std::filesystem::path destination);
		void Read(const nlohmann::json& json, bool ReadDomain = true);
	};
};
