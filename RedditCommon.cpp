#include "RedditCommon.hpp"

namespace RST
{
	void RedditCommon::Read(const std::string& json)
        {
                using namespace SBJSON;

                nlohmann::json root = nlohmann::json::parse(json);

                URL = GetValue<std::string>(root, "url");
                Id = GetValue<std::string>(root, "id");
                Author = GetValue<std::string>(root, "author");
                Permalink = GetValue<std::string>(root, "permalink");
                Domain = GetValue<std::string>(root, "domain"); 
                CreatedUTC = GetValue<time_t>(root, "created_utc");
        }

};

