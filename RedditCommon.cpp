#include "RedditCommon.hpp"

namespace RST
{
	void RedditCommon::Read(const nlohmann::json& json, bool ReadDomain)
	{
		using namespace SBJSON;

		if (!json.contains("url"))
		{
			URL = GetValue<std::string>(json, "link_url");
		}
		else {
			URL = GetValue<std::string>(json, "url");
		}

		Id = GetValue<std::string>(json, "id");
		Author = GetValue<std::string>(json, "author");
		Permalink = GetValue<std::string>(json, "permalink");
		if (ReadDomain)
			Domain = GetValue<std::string>(json, "domain");
		else
			Domain = "RST_NO_DOMAIN_FOUND";
		CreatedUTC = GetValue<time_t>(json, "created_utc");
	}

};

