#include <iostream>
#include "Saver.hpp"
#include "nlohmann/json.hpp"
#include <boost/algorithm/string.hpp>
#include <cstdlib>

using namespace std;
// TODO: Add more options for directory structure E.g /Sub/Post_tite/[Content]
// TODO: Add things like author, permalink and etc to the text files
// TODO: Create GUI for RSA
// TODO: Create a better CMD args scanner or check if you can make it better
// TODO: Implement the rest of the ways for directory structure

CMDArgs* scan_cmd(int argc, char* argv[])	
{

	/*
		Flags:

		-dc: No comments
		-i: Disable Images
		-a [ACCOUNT]: Load specific account
		-t: Disable Text
		-dc: Disable comments
		-l [limit]: Sets the limit of the number of comments, the default being 250 items
		-gt: Get text/self posts and comment bodies - not implemented yet
		-rha: Enable reddit-html-archiver output
	
	*/

	CMDArgs* args = new CMDArgs();
	for (int i = 1; i < argc; i++)
	{
		std::string arg = argv[i];
		//std::cout << "Scanning: " << arg << std::endl;
		if(arg == "-i") {
			args->EnableImages = false;
		} else if(arg == "-t") {
			args->EnableText = false;
		} else if(arg == "-a") {
			if(i + 1 >= argc) {
				std::cout << "Error: Secondary argument for -a option not present" << std::endl;
				delete args;
				return nullptr;
			}
			args->username = argv[i + 1];
			i++;
		}
		else if (arg == "-dc") {
			args->DisableComments = true;
		}
		else if (arg == "-rha") {
			args->RHA = true;
		}
		else if (arg == "-l") {
			if (i + 1 >= argc) {
				std::cout << "Secondary argument for -l option not present" << std::endl;
				delete args;
				return nullptr;
			}

			args->limit = atoi(argv[i + 1]);
			i++;
		}
		else {
			std::cerr << "Error, unkown command: " << argv[i] << std::endl;
			delete args;
			return nullptr;
		}
	}
	return args;
}

int main(int argc, char* argv[])
{
	CMDArgs* args = scan_cmd(argc, argv);
	if (!args)
	{
		return -1;
	}

	Saver s(args);

	if (!s.load_login_info())
	{
		return -1;
	}

	s.init_logs();
	std::cout << "Requesting access to Reddit..." << std::endl;
	State w = s.AccessReddit();
	if (w.http_state != 200)
	{
		std::cout << "Error, failed to get a Reddit access token, " << w.http_state << " : " << w.message << std::endl;
		return -1;
	}
	std::vector<Item*> iv;
	s.AccessPosts(iv, s.args->limit, s.args->DisableComments);

	std::cout << "Processed: " << iv.size() << std::endl;
	if (s.args->EnableText || s.args->EnableImages) {
		std::cout << "Beginning to download content" << std::endl;
		s.download_content(iv, Subreddit); // TODO: Add sorting options to the commandline arguments
	}

	if (s.args->RHA) {
		// TODO: Add filtering options to commandline arguments
		std::cout << "Writing RHA CSVs" << std::endl;
		s.WriteLinkCSV(iv);
	}

	std::cout << "Done." << std::endl;


	return 0;
}
