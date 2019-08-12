# Reddit-Save-Tool

Reddit-Save-Archiver is a Reddit tool where it saves posts, comments, pictures that are in your personal saved.

#Using Reddit-Save-Archiver

Before you start RSA, you need to setup your credentials from https://reddit.com/preferences/apps then apply them into settings.json and apply the necessary program parameters:
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
   
