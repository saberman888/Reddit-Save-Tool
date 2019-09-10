#pragma once

#include "curl/curl.h"
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
#include "boost/algorithm/string.hpp"
#include "base.hpp"
#include <map>
#ifdef _MSC_VER
#include <filesystem>
#elif defined(__GNUC__) && !(defined(__MINGW64__) || defined(__MINGW32__))
#include <experimental/filesystem>
#else
#include <filesystem>
#endif
#include "Reddit.hpp"

class Saver : public RedditAccess
{
public:
	Saver();
	bool scan_cmd(int argc, char* argv[]);
	State RetrieveComments(Item *i);
	State AccessPosts(std::vector<Item*>& saved);
	void WriteLinkCSV(std::vector<Item*> src) { std::vector<std::string> filter; write_links(src, filter); }
	void WriteLinkCSV(std::vector<Item*> src, std::vector<std::string> filter) { write_links(src, filter); }
	bool write_links(std::vector<Item*> src, std::vector<std::string> subfilter);
	void download_content(std::vector<Item*> i);
	bool posts_only;
	bool comments_only;

private:

	State get_saved_items(std::vector< Item* >& sitem, std::string after, bool get_comments);
	State retrieve_comments(Item* i);


};
