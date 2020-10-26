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
#include "RedditObject.hpp"
#include <fstream>
#include <cstdlib>
#include <utility>
#include "sbjson.hpp"

namespace fs = std::filesystem;
using Json = nlohmann::json;

class Saver : public RedditAccess
{
public:
	Saver();
	bool LoadLogins();
	// Gets 100 posts per iteration
	bool GetSaved();
	bool ScanArgs(int argc, char* argv[]);
	fs::path MediaPath;
	std::string after;
	// Handles for Imgur were going to use for imgur links
	ImgurAccess ImgurHandle;

	// Where all the posts we are going to store are going to go
	std::vector<RedditObject> posts;

	bool IsAVideo(Json Post);
	bool IsImage(std::string link);

	State Download(std::string URL);
	bool WriteContent(RedditObject post);
private:
	CMDArgs args;
	State RetrieveSaved();
	bool ParseSaved(const std::string& buffer);
};
