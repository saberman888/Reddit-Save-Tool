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

settings.json can be stored in two places, in the root or in /home/$USER/.config/reddit-saver/ if you're on linux
Once you're done, just call reddit-saver with the -a or --account flag on your username and it should start.

```
reddot-saver -a [ACCOUNT]
```

## Building RSA/RST

RST depends on the following dependencies:

********** [libcURL](https://curl.haxx.se)
* [nlohmann's json](https://github.com/nlohmann/json)
* OpenMP (optional)

For a release build, call -DCMAKE_BUILD_TYPE=Release with cmake and for a debug build use -DCMAKE_BUILD_TYPE=Debug
You can add -DUSE_OPENMP to speed up downloading, but the output is a bit messy

```
cmake -G "COMPILER_HERE"
make
```

## Known Issues / Notes

