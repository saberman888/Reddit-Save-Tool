#include "Image.hpp"

namespace RST
{
		Image::Image(const std::string& json)
		{
			RedditCommon::Read(json);
		}
		
		Image::Image(const std::string& json, std::string 
ImgurClientId) : ImgurClientId(ImgurClientId)
		{
			RedditCommon::Read(json);
		}
		
		std::string Image::GetImage(){
			if(!ImgurClientId.empty()){
				return ImgurAccess::GetImage(URL, 
ImgurClientId);
			} else {
				return URL;
			}
		}
		
		std::string GetImageExtension(State& response)
		{
			auto ContentType = 
splitString(splitString(response.ContentType, ';')[0], '/');
			if(ContentType[0] == "image"){
				std::string extension = "." + ContentType[1];
				return extension;
			} else {
				return "";
			}
		}
		
		bool Image::Write(std::filesystem::path dest){
			auto image = Download(URL);
			if(!image.AllGood()){
				std::cerr << "Error, failed to get image " << 
URL << std::endl;
				std::cerr << image.HttpState << " " << 
image.Message << std::endl;
			
				return false;
			}
			// 
			RST::Write(dest / (Id + GetImageExtension(image)), 
image.buffer);
			return true;
		}
};
