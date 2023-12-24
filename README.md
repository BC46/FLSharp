# Engine Throttle Sync Fix
Fixes a bug that causes the client to not send the correct engine state of the player's ship to the server.

## Installation instructions
Download the latest `EngineThrottleSyncFix.dll` from [Releases](https://github.com/BC46/EngineThrottleSyncFix/releases). Extract the `dll` to the `EXE` folder of your Freelancer installation. Next, open the `dacom.ini` file and append `EngineThrottleSyncFix.dll` to the `[Libraries]` section.

## Build instructions
Build the project using the `NMAKE` tool from Microsoft Visual C++ 6.0 (VC6 or Visual Studio 98) with the included makefile.
This will output the `EngineThrottleSyncFix.dll` file in the `bin` folder.
Building the project using other compilers may require the code to be modified.

## Credits
- [Aingar](https://github.com/Aingar)
  - Identification of the original bug and fix implementation within DiscoveryGC's client hook
- [BC46](https://github.com/BC46)
  - Standalone fix implementation
