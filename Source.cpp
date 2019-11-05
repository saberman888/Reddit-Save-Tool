#include <iostream>
#include "Saver.hpp"
#include "nlohmann/json.hpp"
#include "boost/algorithm/string.hpp"
#include <cstdlib>

using namespace std;
// TODO: Add option to not download images, but output the URLs into a text file
// TODO: Add more options for directory structure E.g /Sub/Post_tite/[Content]
// TODO: Add things like author, permalink and etc to the text files
// TODO: Create GUI for RSA

/*
		Flags:

		-i: Only images
		-a [ACCOUNT]: Load specific account
		-t: Only text
		-e: Get everything
		-dc: Disable comments
		-l [limit]: Sets the limit of the number of comments, the default being 250 items
		-rha: Enable reddit-html-archiver output
		-v/--version: Get version
		-whl/-whitelist [sub,sub] - whitelists a patricular sub
		-bl/-blacklist [sub,sub] - blackists a paticular sub
		-sb/sort_by [sort] - sort media
		-r/-reverse - reverse the saved item list

	*/

int main(int argc, char* argv[])
{

	Saver s;
	bool result = s.scan_cmd(argc, argv);
	if (!result)
	{
		return -1;
	}

	if (!s.load_login_info())
	{
		return -1;
	}

	s.init_logs();
	std::cout << "Requesting access to Reddit..." << std::endl;
	State w = s.AccessReddit();
	//if(true)
		//s.authorize_imgur();
	if (w.http_state != 200)
	{
		std::cout << "Error, failed to get a Reddit access token, " << w.http_state << " : " << w.message << std::endl;
		return -1;
	}
	std::vector<Item*> iv;
	s.AccessPosts(iv);

	if (s.args.reverse)
	{
		std::reverse(std::begin(iv),std::end(iv));
	}

	std::cout << "Processed: " << iv.size() << std::endl;
	if (s.args.EnableText || s.args.EnableImages || s.args.VideosEnabled) {
		std::cout << "Beginning to download content" << std::endl;
		s.download_content(iv); // TODO: Add sorting options to the commandline arguments
	}

	if (s.args.RHA) {
		// TODO: Add filtering options to commandline arguments
		std::cout << "Writing RHA CSVs" << std::endl;
		s.WriteLinkCSV(iv);
	}

	std::cout << "Done." << std::endl;


	return 0;
}
