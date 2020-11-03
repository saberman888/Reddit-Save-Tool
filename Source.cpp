#include <iostream>
#include "Saver.hpp"
#include <cstdlib>

using namespace std;
/*
 * TODOS
 *
 * TODO: Add filtering options for subreddit, nsfw and more
 * TODO: Add getting gifv
 * TODO: Finish adding in Imgur suppoer
 * TODO: Allow ffmpeg command to be changed using the settings file
 * TODO: Add folder organization options for subreddit, nsfw and types.
 * TODO: Add option to keep video or audio file if getting one of them fails
 * TODO: In BasicRequest change response from being a pointer to a normal variable
 * TODO: Add error checking if critical arguments aren't present like --account
 * TODO: Remove base.cpp/.hpp from the project because I no longer need it
 * TODO: Add -DNDEBUG=1 to release builds in CMakeLists.txt
 */
int main(int argc, char* argv[])
{
	BasicRequestRAII braii;
	Saver start(argc, argv);
	return 0;
}
