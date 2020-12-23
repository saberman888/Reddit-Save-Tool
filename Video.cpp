#include "Video.hpp"

namespace RST
{
	Video::Video(const std::string& json) : DashPlaylistUrl(), AudioURL(), 
HasAudio(false)
	{
		Read(json);
		if(CheckForAudio())
		{
			int ending = URL.rfind("/");
			std::string audio_url = URL.substr(0, ending+1);
			if (contains(URL, ".mp4"))
			{
				AudioURL = audio_url + "DASH_audio.mp4";
			}
			else {
				AudioURL = audio_url + "audio";
			}
		}
	}
	
	bool Video::Write(std::filesystem::path dest)
	{
		auto video = Download(URL);
		if(!video.AllGood())
		{
			std::cerr << "Error, Failed to download Video" << 
std::endl;
			std::cerr << video.HttpState << " " << video.Message << 
std::endl;
			return false;
		}
		
		
		if(HasAudio){
			RST::Write(dest / (Id + "_video.mp4"), 
video.buffer);
			auto audio = Download(AudioURL);
			if(!audio.AllGood()){
				std::cerr << "Error, Failed to download Audio" 
<< std::endl;
				std::cerr << audio.HttpState << " " << 
audio.Message << std::endl;
				return false;
			}
			RST::Write(dest / (Id + "_audio.mp4"), audio.buffer);
		} else {
			RST::Write(dest / (Id + ".mp4"), video.buffer);
		}
		Mux(dest / (Id + ".mkv"));
		return true;
	}

	void Video::Read(const std::string& json)
	{
		using namespace SBJSON;

		RedditCommon::Read(json);
		
		nlohmann::json root = nlohmann::json::parse(json);

		auto redditVideo = root.at("media").at("redditvideo");
		URL = GetString(redditVideo, "fallback_url");
		DashPlaylistUrl = GetString(redditVideo, "dash_url");
	}
	
	bool Video::CheckForAudio()
	{
		auto dashDownload = Download(DashPlaylistUrl);
		if(dashDownload.AllGood())
		{
			 
			if(std::regex_search(dashDownload.buffer,
	std::regex("<BaseURL>(DASH_)audio(.mp4)</BaseURL>")))
			{
				return true;
			}
		} else {
			std::string errmsg = 
std::to_string(dashDownload.HttpState) + " " + 
dashDownload.Message;
			throw std::runtime_error(errmsg);
		}
		return false;
	}

	void Video::Mux(std::filesystem::path source)
	{
		std::string filename = Id + ".mkv";
		std::filesystem::path video = source / (Id + "_video.mp4");
		std::filesystem::path audio = source / (Id + "_audio.mp4");
		std::string ffmpegFullCommand;

		ffmpegFullCommand = SearchAndReplace(ffmpegCommand, 
"{video_source}", video.string());
		ffmpegFullCommand = SearchAndReplace(ffmpegCommand, 
"{audio_source}", audio.string());
		ffmpegFullCommand = SearchAndReplace(ffmpegCommand, 
"{destination}", source.string());
		

		std::system(ffmpegFullCommand.c_str());
		std::filesystem::remove(video);
		std::filesystem::remove(audio);

	}
}
