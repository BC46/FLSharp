# FLSharp
FLSharp (FL# for short) is a plugin for the game Freelancer (2003) that aims to apply game-related fixes and improvements using client hooks.
These include:
- Fixed a bug that caused the lights of a trade lane to never turn back on after the trade lane gets disrupted.
- Fixed a bug that caused waypoints to be cleared when the player reaches the coordinates in a different system.
- Fixed a bug that makes the player ship a selectable target for creating waypoints on the nav map.
- Makes the weapon flash particle effect play on all barrels instead of only on the first barrel.
- Fixed a bug for the `one_shot_sound` not playing when firing multi-barrel launchers.
- Fixed a bug for the ammo count not decrementing correctly when firing multi-barrel launchers.
- Fixed a bug that caused the `use_animation` scripts defined in `weapon_equip.ini` to not work, i.e. enables weapon animations.
- Allows a gun's `use_animation` script to be played on the parent (e.g. ship) if the animation name has a leading underscore (`_`); this leading underscore should **not** be included in the cmp file's animation name.
- Fixed a bug that caused the client to not send the correct engine state of the player's ship to the server.
- When playing on a server, ensure the client sends an update:
    - at least every 2 seconds;
    - when engine kill has been toggled;
    - if after every 0.25 seconds the ship's orientation has been changed to some extent.
- Sets the minimum time between client-server updates to 40 milliseconds (or 750 ms while in a trade lane) such that jitter is prevented when playing with a high or inconsistent framerate.
- Allow `ui_music_test` to play when the current background music has finished playing.
- Added support for playing the `ui_interface_test` and `ui_ambiance_test` sounds when adjusting the respective sliders in the options menu.
- Automatically prevent crashes while adjusting the interface and ambience volume sliders in the event that the test sounds are not defined.
- While adjusting the ambience and music volume, mute other background sounds accordingly so that the volume can be fine-tuned more easily.
- Allows for up to 127 selectable resolutions in the general options menu; these are automatically determined based on the user's main monitor resolution.
- Fixes the problem where the general options menu can't distinguish two different resolutions with the same width.
- Makes the default resolution button in the general options menu select the user's main monitor resolution instead of 1024x768.
- Prevent Freelancer from running with resolutions beyond the main display's capabilities in terms of resolution width and height.
- Sets the user's main monitor resolution as the default in-game resolution (assumes widescreen support has already been added by JFLP and HUD Shift).
- Instead of checking if the available resolutions are supported each time the option menu opens, do it only when needed (optimization).
- Allows for the Ctrl + C hotkey to copy text from any Freelancer input box to the clipboard.
- Allows for the Ctrl + V hotkey to paste text from the clipboard to any Freelancer input box.
- Improved the slide-out animation for some UI buttons in the multiplayer menus.
- When the "You must be on friendlier terms to purchase this." message is displayed in the Dealer menus, allow the exact reputation requirement to be printed too. Include `%d` in the IDS (S 1564, `resources.dll`) to make it work.
- Fixed a bug that caused the Freelancer process to not always terminate after closing the game.
- Automatically set rotation lock and auto leveling to the default value when launching to space to prevent control-related issues from occurring (e.g. mouse flight not working).
- Allows infocards for dynamic solars (based on `missioncreatedsolars.ini`) to be displayed in the Current Information window; see [these entries](https://github.com/FLHDE/freelancer-hd-edition/blob/b1c99db03f37d389aac12048b5b8b28e110cf37c/DATA/UNIVERSE/missioncreatedsolars.ini#L46-L54) for an example on how to make infocards work for non-dockable dynamic solars.
- Show a fallback infocard based on the solar's archetype in the Current Information window if the solar itself has no infocard.
- Fixed a startup crash that occurs when Freelancer tries to load a modded/malformed save file.
- Fixed a crash when selecting a modded/malformed save file in the F1 Load Game menu
- Fixed a very rare and inconsistent crash in alchemy.dll (crash offset `0x701b`).
- Fixed waypoints being called "Unknown Object" in the target view and Current Information window.
- Fixed information about player waypoints being printed incorrectly in the Current Information window.
- Fixed ships part of the `fc_uk_grp` faction having a blank faction in the Current Information window.
- Fixed a crash that sometimes happens when closing the Server Filter dialog.
- Fixed a bug that causes the game speed to sometimes suddenly increase when opening the Server Filter dialog.
- Fixed Freelancer and FLServer crashing if a non-existing DLL is loaded via freelancer.ini.
- Shows the actual shield capacity value in the shield infocards (reduced by the offline_threshold value).
- Fixed the equipment and commodity dealer menus not opening when clicking the dealer twice.
- Fixes some crashes that can occur when opening the dealer menus.
- No longer kick players from the server for re-purchasing the same ship.
- Allows the player to enter formation with hostile group members.

More fixes and improvements may be added in the future.

## Installation instructions
Download the latest `FLSharp.dll` from [Releases](https://github.com/BC46/FLSharp/releases). Extract the `dll` to the `EXE` folder of your Freelancer installation. Next, open `dacom.ini` and  `dacomsrv.ini` and in both files append `FLSharp.dll` to the `[Libraries]` section. **Note that FLSharp requires the official 1.1 patch.** Some of the plugin's features may work fine with 1.0 but this is not guaranteed.

Optionally, you can download the [FLSharp.ini](https://github.com/BC46/FLSharp/blob/main/FLSharp.ini) file and place it in the EXE folder. In this file you can specify for every individual feature whether or not it should be enabled. To disable a feature, change its value from `true` to `false`. In almost all cases, having every feature enabled is ideal since these are all quality of life improvements. However, you may want to disable one or more features if you do not like them for whatever reason or if they introduce compatibility issues with other plugins.

If the `FLSharp.ini` file is missing or if it can't be opened, then all features are enabled by default. When an entry in `FLSharp.ini` is missing, then that particular feature will also be enabled by default. In other words, every feature is by default enabled unless stated otherwise.

## Build instructions
Build the project using the `nmake` tool from Visual Studio 2017.
This will output the `FLSharp.dll` file in the `bin` folder.
Building the project using other compilers may require the code to be modified.

## Credits
Thanks to the following people for helping make some of the features possible:

- [adoxa](https://github.com/adoxa)
  - Provided source code of many Freelancer plugins.
- [Aingar](https://github.com/Aingar)
  - Identification of the client-to-server communication improvements and providing code from DiscoveryGC's client hook.
- [Schmackbolzen](https://github.com/Schmackbolzen)
  - Provided assistance with troubleshooting the ship preview rendering bug.
- [Venemon](https://www.moddb.com/members/venemon)
  - Original implementations of the multi-barrel launcher sound and [weapon animation fixes](https://www.moddb.com/mods/weapon-animations). Also provided help with debugging and reverse engineering.
