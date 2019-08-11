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
#include <utility>
#include <any>
#include <algorithm>
#include "boost/algorithm/string.hpp"
#include "base.hpp"
#include <map>
#ifdef _MSC_VER
#include <filesystem>
#elif __GNUC__ && !(defined(__MINGW64__) || defined(__MINGW32__))
#include <experimental/filesystem>
#else
#include <filesystem>
#endif
#include "Reddit.hpp"


// State holds the results of a request



class Saver : public RedditAccess
{
public:
	Saver(CMDArgs* arg);
	~Saver(){}
	/* these toggles enable using old Reddit or printing out the linked urls with the saved urls
	   by default old Reddit and outputting linked urls are disabled */


	State RetrieveComments(Item *i);
	State AccessPosts(std::vector<Item*>& saved, int limit, bool get_comments);
	void WriteLinkCSV(std::vector<Item*> src) { std::vector<std::string> filter; write_links(src, filter); }
	void WriteLinkCSV(std::vector<Item*> src, std::vector<std::string> filter) { write_links(src, filter); }
	bool write_links(std::vector<Item*> src, std::vector<std::string> subfilter);

	bool posts_only;
	bool comments_only;

	// Some stats I want the program to collect while cycling through the items
	int images, self_posts, links, videos, comments;

private:
	/*
		SaveToggle saves or unsaved depending if remove is enabled or not
	
	*/
	State SaveToggle(std::string fullname, bool remove);
	// Just gets the user's save items
	State get_saved_items(std::vector< Item* >& sitem, int limit, std::string after, bool prev_continue, bool get_comments);
	State retrieve_comments(Item* i);
	// AccessSaved is the function to retrieve saved items, and they're stored in std::vector<Item*>& saved
	State AccessSaved(std::vector<Item*>& saved, int limit, std::string after, bool prev_continue, bool get_comments);
public:
	void download_content(std::vector<Item*> i, Sort s);

};