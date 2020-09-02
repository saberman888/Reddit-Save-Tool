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

	// 1000 is the max number of elements I can pull from a user's saved
	std::vector<RedditObject> content;

	bool IsAVideo(Json Post);
	bool IsImage(std::string link);
	void Download(std::string URL);
	void Write(fs::path filepath, std::string filename);
private:
	CMDArgs args;
	void RetrieveSaved();
	bool ParseSaved();
};
