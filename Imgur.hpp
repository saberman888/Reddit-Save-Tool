#pragma once

#include "BasicRequest.hpp"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>

class ImgurAccess : public BasicRequest
{
public:
	ImgurAccess(std::string ClientId) : ClientId(ClientId) {}
	std::string GetImage(std::string ImageHash);
	std::vector<std::string> GetAlbum(std::string AlbumHash);
	bool IsImage(std::string URL);
	bool IsAlbum(std::string URL);
private:
	std::string ClientId;
	void ImgurGet(std::string URL);

	std::string ParseImage(std::string json);
	std::vector<std::string> ParseAlbum(std::string json);

};