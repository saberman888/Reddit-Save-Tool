# Reddit-Save-Tool/Reddit-Save-Archiver

Reddit-Save-Archiver is a Reddit tool where it saves posts, comments, pictures that are in your personal saved.

## Building RSA/RST

For most people compiling RSA can be accomplished by running cmake then make.

```
cmake -G "COMPILER_HERE"
make
```

However for some compilers, the -DUSE_EXP_FS=1 flag is needed. This is because the way RSA detects std::filesystem may not work for some compilers.

```
cmake -G "COMPILER_HERE" -DUSE_EXP_FS=1
```

Another cmake option you can add, is the USE_HOME_DIR. This flag stores media and logs in ~/RSA/ on Linux.

### RSA depends on:
1. Boost's headers
2. Libcurl
3. nlohmann's json

## Using RSA/RST

Before you start RSA, you need to setup your credentials from https://www.reddit.com/prefs/apps/ , apply them into settings.json and apply then the necessary program parameters:

```
{
    "accounts": [
        {
            "client_id": "cid_here",
            "password": "password_here",
            "secret": "secret_here",
            "user_agent": "useragent_here",
            "username": "username_here"
        }
    ],
    "imgur" : {
      "client_id": "cid"
    }

}

```

Run RSA without any flags if you want everything thats scanned

```
	Flags:

		-i: Disable Images
		-a [ACCOUNT]: Load a specific account
		-t: Disable Text
		-dc: Disable comments
		-ect : Enable the retrieval of comment threads
		-l [limit]: Sets the limit of the number of items, the default being 1000 items
		-rha: Enable reddit-html-archiver output
		-v/-version: Version
		-whl/-whitelist [sub]: Whitelist a paticular sub and block the others
		-bl/-blacklist [sub]: Black list a paticular sub
		-h/-help: help
		-sb/sortby [sort]: Sorts downloaded media by: title,subreddit,id or unsorted; with the default being subreddit
		-r/-reverse: reverse the list of items collected from save
    		-uw [user,user]: Enable whitelisting users
    		-ub [user,user]: Enable blacklisting of users
    		-bd [domain,domain] : Enable blacklisting of domain names
    		-wd [domain,domain] : Enable whitelisting of domain names
		-vb : Enable output of more logs
```

## Imgur on RSA/RST
As of October 27, RSA is able to download albums and images from Imgur using It's API. This can be enabled by adding a client id in settings.json
To get this ID, go to your Applications in your account settings on Imgur and register RSA.

## Known Issues / Notes

1. Not really an issues/bug, but RSA doesn't support downloading video, but I want to try to support it in the future
2. The -RHA option doesn't always work when processing a large scale of items, but I could be wrong, it might not work at all.
3. It's important to know that, the ability to retrieve full comment threads is not fully implemented.

## Things I plan to do to RSA/RST

1. Fix the aformentioned issues
2. Add more options for directory structure E.g /Sub/Post_tite/[Content]
3. Create GUI for RSA
