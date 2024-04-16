# FLSharp
FLSharp (FL# for short) is a client-side plugin for the game Freelancer (2003) that aims to apply game-related fixes and improvements to the game's code.
These include:
- Fixed a bug that causes the client to not send the correct engine state of the player's ship to the server.
- Fixed a bug that causes set waypoints to be cleared despite the player being in a different system.
- Fixed a bug for the `one_shot_sound` not playing when firing multi-barrel launchers.
- When playing on a server, ensure the client sends an update:
    - at least every 2 seconds;
    - when engine kill has been toggled;
    - if after every 0.4 seconds the ship's orientation has been changed to some extent.
- Sets the minimum time between client-server updates to 75 milliseconds such that jitter is prevented when playing with a high framerate.

More fixes and improvements may be added in the future.

## Installation instructions
Download the latest `FLSharp.dll` from [Releases](https://github.com/BC46/FLSharp/releases). Extract the `dll` to the `EXE` folder of your Freelancer installation. Next, open the `dacom.ini` file and append `FLSharp.dll` to the `[Libraries]` section.

## Build instructions
Build the project using the `NMAKE` tool from Microsoft Visual C++ 6.0 (VC6 or Visual Studio 98) with the included makefile.
This will output the `FLSharp.dll` file in the `bin` folder.
Building the project using other compilers may require the code to be modified.

## Credits
- [Aingar](https://github.com/Aingar)
  - Identification of the original bugs/improvements and implementations within DiscoveryGC's client hook
- [BC46](https://github.com/BC46)
  - Standalone implementations
