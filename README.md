# OnlyUpMultiplayerExe
> Have you ever wanted to walk up to the clouds? Embark on an exciting journey in Only UP! Exploring a huge world full of secrets and mysteries, you have to get as high as possible, and the most interesting starts above the clouds...

This is the launcher for the OnlyUp multiplayer mod (This mod requires the DLL file to work)

## How to use
To use this mod, please also install [the DLL file](https://github.com/NotNanook/OnlyUpMultiplayerDLL) required for this mod to work.
After you got both files donwloaded, simply drag an drop them into your main OnlyUp folder. (E.g. D:\SteamLibrary\steamapps\common\Only Up!)

Then launch your games by simply running the installed `OnlyUpMultiplayer.exe`. It will start and inject `OnlyUpMultiplayer.dll` into the OnlyUp process.

Open the mod menu in-game, with RSHFIT

## Server hosting
To use this mod you have to host your own server. 
1. Download the server file (Dockerfile included), onto your server
2. Build the container `docker build -t OnlyUpMultiplayer .`
3. Run the container `docker run -d -p 50001:50001/udp OnlyUpMultiplayer` (Ports are important)

## Known Issues
This mod **doesnt work in fullscreen** and you cant resize the windows either

## Support and Contribution
I always welcome help in each and every way. If you want to donate, [here](https://ko-fi.com/notnanook) is my ko-fi page. If you want to help improve the mod, you are always welcome to create a pull request or message me on my Discord @notnanook for tips or improvements
