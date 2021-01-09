#include "Comment.hpp"

namespace RST
{
	Comment::Comment(const nlohmann::json& json) {
		Comment::Read(json);
	}

	void Comment::Read(const nlohmann::json& json, bool ReadDomain) {
		RedditCommon::Read(json, false);
		Text = SBJSON::GetValue<std::string>(json, "body");
	}

	bool Comment::Write(std::filesystem::path destination) {
		std::string filename = (destination / (Id + ".txt")).string();
		std::fstream out(filename, std::ios::out);
		out << "On Post: " << Title << std::endl;
		out << "Author: " << Author << std::endl;
		out << "Date: " << BasicRequest::UTCToString(CreatedUTC, "%Y-%m-%d @ %H:%M %p") << std::endl;
		out << "ID: " << Id << std::endl;
		out << "Permalink: " << Permalink << std::endl;
		out << Text << std::endl;
		return true;
	}
};
