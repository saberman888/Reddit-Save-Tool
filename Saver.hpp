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

#if defined(__cpp_lib_filesystem)
#include <filesystem>
namespace fs = std::filesystem;
#elif defined(__cpp_lib_experimental_filesystem)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#elif defined(USE_EXP_FS)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#error "No filesystem support found :("
#endif


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
	bool LoadLogins();
	bool scan_cmd(int argc, char* argv[]);

	bool IsLoggedIn;
	std::string after;
	fs::path MediaPath;
	CMDArgs args;

	// 1000 is the max number of elements I can pull from a user's saved
	std::vector<Json> content;

	bool IsAVideo(Json Post);
	bool IsImage(std::string link);

	State RetrieveSaved(std::string& buffer, std::string after);
	void ParseSaved(std::string json);

	State Download(std::string URL, std::string& buffer, std::string& ContentType);
};
