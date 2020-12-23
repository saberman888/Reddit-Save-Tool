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
			virtual bool Write(std::filesystem::path dest);
		protected:
			virtual void Read(const std::string& json);

	};
};
