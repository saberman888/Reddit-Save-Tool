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
	constexpr std::string_view ffmpegCommand = "ffmpeg -y -i \
{video_source} {audio_source} -c copy {destination_video}";
	class Video : public RedditCommon
	{
		public:
			Video(const std::string& json);
			bool Write(std::filesystem::path destination);
		private:
			std::string DashPlaylistUrl, AudioURL;
			bool HasAudio;
			
			bool CheckForAudio();
			void Mux(std::filesystem::path destination);
			void Read(const std::string& json);
	};
};
