#pragma once

#include "curl/curl.h"
#include "BasicRequest.hpp"
#include <string>
#include <vector>

class ImgurAccess
{
public:
	ImgurAccess(std::string ClientId) : ClientId(ClientId) {}
	std::string ClientId;
	State retrieve_album_images(std::string album_id, std::vector<std::string>& URLs);
	State retrieve_imgur_image(std::string imghash, std::string& URL);
	State download_item(const char* URL, std::string& buf);
};