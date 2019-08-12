# Reddit-Save-Tool/Reddit-Save-Archiver

Reddit-Save-Archiver is a Reddit tool where it saves posts, comments, pictures that are in your personal saved.

## Building RSA/RST

Building RSA is relatively simple by just configuring cmake to your preffered compiler/platform and compiling it through the generated Makefile:

```
cmake -G "COMPILER_HERE"
```

### RSA depends on:
1. Boost's headers
2. Libcurl

## Using RSA/RST

Before you start RSA, you need to setup your credentials from https://www.reddit.com/prefs/apps/ then apply them into settings.json and apply the necessary program parameters:

```
{"accounts": [{"client_id":"CLIENT_ID_HERE","PASSWORD_HERE":"psw","secret":"SECRET_HERE","user_agent":"USER AGENT HERE","username":"USERNAME_HERE"}]}
```

Run RSA without any flags if you want everything thats scanned

```
		Flags:

		-dc: Disable comments from any of the self/link posts
		-i: Disable Images
		-a [ACCOUNT]: Load a specific account
		-t: Disable Text
		-l [limit]: Sets the limit of the number of comments, the default being 250 items
		-gt: Disable text/self posts and direct save comments - not implemented yet
		-rha: Enable reddit-html-archiver output
```
   
## Known Issues

1. The -l [limit] option doesn't always work
2. Running RSA without any valid settings.json will cause the program to crash or generate a segment fault


## Things I plan to do to RSA/RST

1. Fix the aformentioned issues
2. Add more options for directory structure E.g /Sub/Post_tite/[Content]
2. Add things like author, permalink and etc to the text files
3. Create GUI for RSA
4. Create a better CMD args scanner or check if you can make it better
5. Implement the rest of the ways for directory structure
