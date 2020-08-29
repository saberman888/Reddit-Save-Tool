#include "Imgur.hpp"

std::string ImgurAccess::GetImage(std::string ImageHash)
{
	std::string json;
	ImgurGet(ImageHash);
	if (response.HttpState == 200)
	{
		return ParseImage(response.buffer);
	}
}

std::vector<std::string> ImgurAccess::GetAlbum(std::string Album)
{
	std::string endpoint = "/a/" + Album;
	std::vector<std::string> Images;
	ImgurGet(endpoint);
	if (response.HttpState == 200)
	{
		Images = ParseAlbum(response.buffer);
	}
	return Images;
}

bool ImgurAccess::IsImage(std::string URL)
{
	return (URL.rfind("https://imgur.com/", 0) != std::string::npos);
}

bool ImgurAccess::IsAlbum(std::string URL)
{
	return (URL.rfind("https://imgur.com/a/", 0) != std::string::npos);
}

void ImgurAccess::ImgurGet(std::string endpoint)
{
	std::string URL = "https://api.imgur.com";

	ImgurHandle.Setup(URL, &response);
	std::string ImgurHeader =
		"Authorization: Client-ID "
		+ ClientId;

	ImgurHandle.SetHeaders(ImgurHeader);
	ImgurHandle.SetOpt(CURLOPT_FOLLOWLOCATION, 1L);
	ImgurHandle.SetOpt(CURLOPT_SSL_VERIFYPEER, 0L);
	ImgurHandle.SendRequest();
	ImgurHandle.Cleanup();
}


std::string ImgurAccess::ParseImage(std::string json)
{
	std::string data;
	try {
		nlohmann::json root = nlohmann::json::parse(json);
		if (root.contains("data"))
		{
			data = root.at("data").at("link").get <std::string>();
		}
	}
	catch (nlohmann::json::out_of_range&) {
		throw;
	}
	return data;
}
std::vector<std::string> ImgurAccess::ParseAlbum(std::string json)
{
	std::vector<std::string> URLs;
	try {
		nlohmann::json root = nlohmann::json::parse(json);
		nlohmann::json images = root.at("data").at("images");
		for (auto& elem : images)
		{
			URLs.push_back(elem.at("link").get<std::string>());
		}
	}
	catch (nlohmann::json::exception&) {
		throw;
	}
	return URLs;
}
