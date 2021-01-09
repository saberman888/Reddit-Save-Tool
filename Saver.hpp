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
#include <fstream>
#include <cstdlib>
#include <utility>
#include "sbjson.hpp"
#include <cassert>
#include "base.hpp"
#include <memory>
#include <cassert>

#include "Galleries.hpp"
#include "Video.hpp"
#include "Image.hpp"
#include "SelfPost.hpp"
#include "Comment.hpp"

namespace fs = std::filesystem;
typedef nlohmann::json Json;

namespace RST
{

	class Saver : public RedditAccess
	{
	public:
		Saver(int argc, char* argv[]);
	private:
		bool LoadConfig();
		bool LoadLogins(std::string config);
		// Gets 100 posts per iteration
		bool GetSaved();
		fs::path MediaPath;
		std::string after, ImgurClientId;

		// Where all the posts we are going to store are going to go
		std::vector< std::shared_ptr<RedditCommon*> > posts;
		std::map<std::string, std::string> args;

		/*
		* Scans argc and argv for arguments and stores them in args
		*/
		bool ScanOptions(int argc, char* argv[]);
		/*
		 * Gets an argument option that has an array of items and returns a vector
		 */
		std::vector<std::string> GetListOp(std::string option);
		State RetrieveSaved();
		bool ParseSaved(const std::string& buffer);

		/*
			IsBlocked checks returns false if the post, or PostData, fits the criteria of what posts the user wants.
			e.g --domains i.imgur.com is passed, and isBlocked will return true if the post is an i.imgur.com domain
		*/
		bool IsBlocked(const nlohmann::json& PostData);
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
