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
#include <string>

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
#include <cassert>

namespace fs = std::filesystem;
typedef nlohmann::json Json;

class Saver : public RedditAccess
{
public:
	Saver(int argc, char* argv[]);
private:
	bool LoadLogins();
	// Gets 100 posts per iteration
	bool GetSaved();
	fs::path MediaPath;
	std::string after;
	// Handles for Imgur were going to use for imgur links
	ImgurAccess ImgurHandle;

	// Where all the posts we are going to store are going to go
	std::vector<RedditObject> posts;
	std::map<std::string, std::string> args;


	State Download(std::string URL);
	bool WriteContent(RedditObject post);
	bool ScanOptions(int argc, char* argv[]);
	State RetrieveSaved();
	bool ParseSaved(const std::string& buffer);
};

template<typename T>
inline bool find(std::map<T, T>& lhs, std::string rhs)
{
	return !(lhs.find(rhs) == lhs.end());
}

inline void dump(const nlohmann::json& data, std::string filename)
{
  std::fstream out(filename, std::ios::out);
  out << data.dump(4);
}

inline void dump(const std::string& data, std::string filename)
{
  std::fstream out(filename, std::ios::out);
  out << data;
}
