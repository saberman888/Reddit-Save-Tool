# Reddit-Save-Tool/Reddit-Save-Archiver

Reddit-Save-Archiver is a Reddit tool where it downloads, images, galleries, albums, comments and text posts from your Reddit account's saved list.

The first step to running RST, is to create an app in your Reddit account's preferences and copy the credentials in to the configuration file, settings.json
You can also add a Imgur client id, which would help  in getting imgur images which, aren't linked directly, are albums or 18+. You can get this id from https://imgur.com/account/settings/apps

An example settings.json using two accounts would look like this:
```
{
    "accounts": [
        {
            "client_id": "client id",
            "password": "mypassword",
            "secret": "mysecret",
            "useragent": "provided UserAgent",
            "username": "account 1 "
        },
        {
            "client_id": "client id",
            "password": "mypassword",
            "secret": "mysecret",
            "useragent": "provided UserAgent",
            "username": "account 2"
        }

    ],
    "imgur_client_id": "IMGUR CLIENT ID"
}
```
Once you're done, just call reddit-saver with the -a or --account flag on your username and it should start.

```
reddot-saver -a [ACCOUNT]
```

## Building RSA/RST

For a release build, simply calling cmake will do. However, if you want a debug build, add -DCMAKE_BUILD_TYPE=Debug and -DCURL_VERBOSE=1 of you want cURL verbose enabled.
To have a dependency error free compilation, you're going to need [libcURL](https://curl.haxx.se) and [nlohmann's json](https://github.com/nlohmann/json) header library before initiating cmake configuration.

```
cmake -G "COMPILER_HERE"
make
```

## Known Issues / Notes
1. Reddit is having an issue where retrieving videos' video or audio could result in 403. I have yet to figure out a work around.

