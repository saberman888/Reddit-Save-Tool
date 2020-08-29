#pragma once

#include "BasicRequest.hpp"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>

class ImgurAccess
{
public:
	ImgurAccess(std::string ClientId) : ClientId(ClientId) {}
	void GetImage(std::string ImageHash, std::string& buffer);
	void GetAlbum(std::string AlbumHash, std::vector<std::string>& Images);
	bool IsImage(std::string URL);
	bool IsAlbum(std::string URL);
	State response;
private:
	std::string ClientId;
	BasicRequest ImgurHandle;
	void ImgurGet(std::string URL);

	std::string ParseImage(std::string json);
	std::vector<std::string> ParseAlbum(std::string json);

};