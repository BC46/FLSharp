# FLSharp
FLSharp (FL# for short) is a plugin for the game Freelancer (2003) that aims to apply game-related fixes and improvements to the game's code.
These include:
- Fixed a bug that caused the lights of a trade lane to never turn back on after the trade lane got disrupted.
- Fixed a bug that caused waypoints to be cleared when the player reaches the coordinates in a different system.
- Fixed a bug that makes the player ship a selectable target for creating waypoints on the nav map.
- Makes the weapon flash particle effect play on all barrels instead of only on the first barrel.
- Fixed a bug for the `one_shot_sound` not playing when firing multi-barrel launchers.
- Fixed a bug for the ammo count not decrementing correctly when firing multi-barrel launchers.
- Fixed a bug that caused the `use_animation` entries in `weapon_equip.ini` to not work, i.e. enables weapon animations.
- Allows a gun's `use_animation` entry to be played on the parent (e.g. ship) if the animation name has a leading underscore (`_`); this leading underscore should **not** be included in the cmp file's animation name.
- Fixed a bug that caused the client to not send the correct engine state of the player's ship to the server.
- When playing on a server, ensure the client sends an update:
    - at least every 2 seconds;
    - when engine kill has been toggled;
    - if after every 0.25 seconds the ship's orientation has been changed to some extent.
- Sets the minimum time between client-server updates to 40 milliseconds (or 750 ms while in a trade lane) such that jitter is prevented when playing with a high or inconsistent framerate.
- Allow `ui_music_test` to play when the current background music has finished playing.
- Added support for playing the `ui_interface_test` and `ui_ambiance_test` sounds when adjusting the respective sliders in the options menu; these sounds should be defined in `interface_sounds.ini`.
- Automatically prevent crashes while adjusting the interface and ambience volume sliders in the event that the test sounds are not defined.
- While adjusting the ambience and music volume, mute other background sounds accordingly so that the volume can be fine-tuned more easily.
- Allows up to 127 selectable resolutions in the general options menu; these are automatically determined based on the user's main monitor resolution.
- Fixes the problem where the general options menu can't distinguish two different resolutions with the same width.
- Makes the default resolution button in the general options menu select the user's main monitor resolution instead of 1024x768.
- Prevent Freelancer from running with resolutions beyond the main display's capabilities in terms of horizontal and vertical resolution.
- Sets the user's main monitor resolution as the default in-game resolution.
- Instead of checking if the available resolutions are supported each time the option menu opens, do it only when needed (optimization).
- Allows for the Ctrl + C hotkey to copy text from any Freelancer input box to the clipboard.
- Allows for the Ctrl + V hotkey to paste text from the clipboard to any Freelancer input box.
- Improved the slide-out animation for some UI buttons in the multiplayer menus.
- When the "You must be on friendlier terms to purchase this." message is displayed in the Dealer menu, allow the reputation requirements to be printed too. Include `%d` in the IDS (S 1564, `resources.dll`) to make it work.

More fixes and improvements may be added in the future.

## Installation instructions
Download the latest `FLSharp.dll` from [Releases](https://github.com/BC46/FLSharp/releases). Extract the `dll` to the `EXE` folder of your Freelancer installation. Next, open both the `dacom.ini` and `dacomsrv.ini` files and append `FLSharp.dll` to the `[Libraries]` sections.

## Build instructions
Build the project using the `NMAKE` tool from Microsoft Visual C++ 7.0 (VC7) + the VC6 runtime libraries.
This will output the `FLSharp.dll` file in the `bin` folder.
Building the project using other compilers may require the code to be modified.

## Credits
- [Aingar](https://github.com/Aingar)
  - Identification of the client-to-server communication bugs/improvements and implementations within DiscoveryGC's client hook
- [BC46](https://github.com/BC46)
  - Standalone implementations
- [Venemon](https://github.com/Venemon)
  - Original implementations of the multi-barrel launcher sound and [weapon animation fixes](https://www.moddb.com/mods/weapon-animations)
