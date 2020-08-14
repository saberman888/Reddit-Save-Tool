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
#include "boost/algorithm/string.hpp"
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


class Saver : public RedditAccess
{
public:
	Saver();
	bool LoadLogins();
	bool scan_cmd(int argc, char* argv[]);
	State RetrieveComments(Item *i);
	State AccessPosts(std::vector<Item*>& saved);
	void WriteLinkCSV(std::vector<Item*> src) { write_links(src); }
	void download_content(std::vector<Item*> i);

private:
	CMDArgs args;
	State get_saved_items(std::vector< Item* >& sitem, std::string after);
	State retrieve_comments(Item* i);
};
