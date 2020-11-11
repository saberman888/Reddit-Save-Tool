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
#include <memory>

#include "curl/curl.h"
#include "Reddit.hpp"
#include "BasicRequest.hpp"
#include <filesystem>
#include "RedditObject.hpp"
#include <fstream>
#include <cstdlib>
#include <utility>
#include "sbjson.hpp"
#include <cassert>
#include "base.hpp"

namespace fs = std::filesystem;
typedef nlohmann::json Json;

namespace RST
{

	class Saver : public RedditAccess
	{
	public:
		Saver(int argc, char* argv[]);
	private:
		bool LoadLogins();
		// Gets 100 posts per iteration
		bool GetSaved();
		fs::path MediaPath;
		std::string after, ImgurClientId;

		// Where all the posts we are going to store are going to go
		std::vector<RedditObject> posts;
		std::map<std::string, std::string> args;


		State Download(std::string URL);
        bool DownloadImage(RedditObject& post, fs::path destination, std::string filename = std::string());

		bool WritePost(RedditObject& post);

        /*
         * Scans argc and argv for arguments and stores them in args
         */
		bool ScanOptions(int argc, char* argv[]);
        /*
         * Gets an argument option that has an array of items and returns a vector
         */
    std::vector<std::string> GetListOp(std::string option);
    void FilterPosts();
		State RetrieveSaved();
		bool ParseSaved(const std::string& buffer);
	};


	inline static void dump(const nlohmann::json& data, std::string filename)
	{
	  std::fstream out(filename, std::ios::out);
	  out << data.dump(4);
	}

	inline static void dump(const std::string& data, std::string filename)
	{
	  std::fstream out(filename, std::ios::out);
	  out << data;
	}
}
