#pragma once

#include "BasicRequest.hpp"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>
#include "base.hpp"
namespace ImgurAccess
{ 
    inline bool IsAlbum(std::string URL)
    {
        return (URL.find("https://imgur.com/a/") != std::string::npos);
    }

    inline bool IsImgurLink(std::string URL)
    {
        return (URL.find("https://imgur.com/",0) != std::string::npos);
    }
    
    inline static std::string GetHash(std::string URL)
    {
        return RST::splitString(URL, '/').back();
    }
    
    inline bool IsDirect(std::string URL)
    {
        return URL.find("i.imgur.com/",0) != std::string::npos;
    }
    
    std::vector<std::string> GetAlbum(std::string AlbumHash, std::string ClientId); 
    std::string GetImage(std::string ImageHash, std::string ClientId);

}
