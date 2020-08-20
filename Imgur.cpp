#include "Imgur.hpp"

State ImgurAccess::GetImage(std::string Image, std::string& ReturnURL)
{
	std::string json;

	State GetResult = ImgurGet(Image, json);
	if (GetResult.http_state == 200)
	{
		ReturnURL = ParseImage(json);
	}
	return GetResult;
}

State ImgurAccess::GetAlbum(std::string Album, std::vector<std::string>& Images)
{
	std::string endpoint = "/a/" + Album;
	std::string json;
	State GetResult = ImgurGet(endpoint, json);
	if (GetResult.http_state == 200)
	{
		Images = ParseAlbum(json);
	}
	return GetResult;
}

bool ImgurAccess::IsImage(std::string URL)
{
	return (URL.rfind("https://imgur.com/", 0) != std::string::npos);
}

bool ImgurAccess::isAlbum(std::string URL)
{
	return (URL.rfind("https://imgur.com/a/", 0) != std::string::npos);
}

State ImgurAccess::ImgurGet(std::string endpoint, std::string& buffer)
{
	State result;
	std::string URL = "https://api.imgur.com";

	ImgurHandle.Setup(URL);
	std::string ImgurHeader =
		"Authorization: Client-ID "
		+ ClientId;

	ImgurHandle.SetHeaders(ImgurHeader);
	ImgurHandle.SetOpt(CURLOPT_FOLLOWLOCATION, 1L);
	ImgurHandle.SetOpt(CURLOPT_SSL_VERIFYPEER, 0L);
	ImgurHandle.WriteTo(buffer);
	result = ImgurHandle.SendRequest();
	return result;
}


std::string ImgurAccess::ParseImage(std::string json)
{
	try {
		nlohmann::json root = nlohmann::json::parse(json);
		if (root.contains("data"))
		{
			return root.at("data").at("link").get <std::string>();
		}
	}
	catch (nlohmann::json::out_of_range& e) {
		throw;
	}
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
	catch (nlohmann::json::exception& e) {
		throw;
	}
}


