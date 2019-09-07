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
3. nlohmann's json

## Using RSA/RST

Before you start RSA, you need to setup your credentials from https://www.reddit.com/prefs/apps/ , apply them into settings.json and apply then the necessary program parameters:

```
{"accounts": [{"client_id":"CLIENT_ID_HERE","password":"PASSWORD_HERE","secret":"SECRET_HERE","user_agent":"USER AGENT HERE","username":"USERNAME_HERE"}]}
```

Run RSA without any flags if you want everything thats scanned

```
		Flags:

		-i: Disable Images
		-dc: Disable comments
		-a [ACCOUNT]: Load a specific account
		-t: Disable Text
		-l [limit]: Sets the limit of the number of comments, the default being 1000 items
		-RHA: Enable reddit-html-archiver output
		-whl/-whitelist [sub]: Whitelist a paticular sub and block the others
		-bl/-blacklist [sub]: Black list a paticular sub
		-h/-help: help
		-v/-version: Version
```
   
## Known Issues / Notes

1. Not really an issues/bug, but RSA doesn't support downloading video, but I want to try to support it in the future
2. The -RHA option doesn't always work when processing a large scale of items, but I could be wrong.

## Things I plan to do to RSA/RST

1. Fix the aformentioned issues
2. Add more options for directory structure E.g /Sub/Post_tite/[Content]
2. Add things like author, permalink and etc to the text files
3. Create GUI for RSA
4. Implement the rest of the ways for directory structure
