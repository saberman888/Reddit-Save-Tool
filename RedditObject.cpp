#include "RedditObject.hpp"

std::string RedditObject::GetAudioUrl()
{
	return URL + "/DASH_audio.mp4";
}

std::string RedditObject::GetVideoUrl()
{
	return URL + "/DASH_" + std::to_string(VideoInfo.height)+".mp4";
}

void RedditObject::MuxVideo(std::string source, std::string dest)
{
    std::string filename = Id + ".mkv";
    
    std::string ffmpegCommand = "ffmpeg -y -i ";
		ffmpegCommand.append(source); ffmpegCommand.append("/video.mp4 -i ");
		ffmpegCommand.append(source); ffmpegCommand.append("/audio.mp4 -c copy ");
		ffmpegCommand.append(dest); ffmpegCommand.append("/" + filename);
    std::system(ffmpegCommand.c_str());
}

void RedditObject::Write(fs::path filepath, std::string filename, const std::string& buffer)
{
	fs::create_directories(filepath);
	fs::path fullpath = filepath / filename;
	std::fstream out(fullpath.string(), std::ios::out | std::ios::binary);
	out << buffer;
}
