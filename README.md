# FLSharp
FLSharp (FL# for short) is a plugin for the game Freelancer (2003) that aims to apply game-related fixes and improvements to the game's code.
These include:
- Fixed a bug that causes the client to not send the correct engine state of the player's ship to the server.
- Fixed a bug that causes set waypoints to be cleared despite the player being in a different system.
- Fixed a bug that makes the player ship a selectable target for creating waypoints on the nav map.
- Fixed a bug for the `one_shot_sound` not playing when firing multi-barrel launchers.
- Fixed a bug for the ammo count not decrementing correctly when firing multi-barrel launchers.
- When playing on a server, ensure the client sends an update:
    - at least every 2 seconds;
    - when engine kill has been toggled;
    - if after every 0.4 seconds the ship's orientation has been changed to some extent.
- Sets the minimum time between client-server updates to 75 milliseconds such that jitter is prevented when playing with a high framerate.
- Allow `ui_music_test` to play when the current background music has finished playing.
- Added support for playing the `ui_interface_test` and `ui_ambiance_test` sounds when adjusting the respective sliders in the options menu.
- Automatically prevent crashes while adjusting the interface and ambience volume sliders in the event that the test sounds are not defined.
- While adjusting the ambience and music volume, mute other background sounds accordingly so that the volume can be fine-tuned more easily.
- Allows up to 127 selectable resolutions in the general options menu; these are automatically determined based on the user's main monitor resolution.
- Fixes the problem where the general options menu can't distinguish two different resolutions with the same width.
- Makes the default resolution button in the general options menu select the user's main monitor resolution instead of 1024x768.
- Prevent Freelancer from running with resolutions beyond the main display's capabilities.
- Sets the user's main monitor resolution as the default in-game resolution.
- Instead of checking if the available resolutions are supported each time the option menu opens, do it only when needed (optimization).

More fixes and improvements may be added in the future.

## Installation instructions
Download the latest `FLSharp.dll` from [Releases](https://github.com/BC46/FLSharp/releases). Extract the `dll` to the `EXE` folder of your Freelancer installation. Next, open both the `dacom.ini` and `dacomsrv.ini` files and append `FLSharp.dll` to the `[Libraries]` sections.

## Build instructions
Build the project using the `NMAKE` tool from Microsoft Visual C++ 6.0 (VC6) with the included makefile.
This will output the `FLSharp.dll` file in the `bin` folder.
Building the project using other compilers may require the code to be modified.

## Credits
- [Aingar](https://github.com/Aingar)
  - Identification of the client-to-server communication bugs/improvements and implementations within DiscoveryGC's client hook
- [BC46](https://github.com/BC46)
  - Standalone implementations
