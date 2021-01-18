#include <iostream>
#include "Saver.hpp"
#include <cstdlib>

using namespace std;
/*
 * TODOS
 *
 * TODO: Add filtering options for subreddit, nsfw and more
 * TODO: Allow ffmpeg command to be changed using the settings file
 * TODO: Add folder organization options for subreddit, nsfw and types.
 * TODO: Add option to keep video or audio file if getting one of them fails
 * TODO: In BasicRequest change response from being a pointer to a normal variable
 */
int main(int argc, char* argv[])
{
	RST::Saver start(argc, argv);
	return 0;
}
