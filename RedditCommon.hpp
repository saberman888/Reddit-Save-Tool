#pragma once

#include <string>
#include <ctime>
#include "sbjson.hpp"
#include <filesystem>


namespace RST
{
	class RedditCommon
	{
	public:
		std::string URL, Id, Author, Permalink, Title, Domain;
		time_t CreatedUTC;
		virtual bool Write(std::filesystem::path dest) = 0;
	protected:
		virtual void Read(const nlohmann::json& json, bool ReadDomain = true);

	};
};
