#pragma once


#include <vector>
#include <iosfwd>
#include <sstream>
#include <stdlib.h>
#include <ratio>
#include <iomanip>
#include <vector>
#include <cctype>
#include <algorithm>
#include <iterator>
#include <map>

#include "curl/curl.h"
#include "base.hpp"
#include "Reddit.hpp"
#include "BasicRequest.hpp"
#include <filesystem>

namespace fs = std::filesystem;

#if defined(unix) || defined(_unix)
constexpr bool IsUnixBased = true;
#else
constexpr bool IsUnixBased = false;
#endif

using Json = nlohmann::json;
class Saver : public RedditAccess
{
public:
	Saver();
	~Saver() { delete imgur; }
	bool LoadLogins();
	bool scan_cmd(int argc, char* argv[]);

	bool IsLoggedIn;
	std::string after;
	fs::path MediaPath;
	CMDArgs args;
	ImgurAccess* imgur;
	// 1000 is the max number of elements I can pull from a user's saved
	std::vector<Json> content;

	bool IsAVideo(Json Post);
	bool IsImage(std::string link);

	void RetrieveSaved(std::string after);
	void ParseSaved(std::string json);

	void Download(std::string URL);
};
