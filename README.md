crossbridge-swc-lua
===================

Modified version of crossbridge lua to better support running user code for Platform Racing 3.

## Building the library

1. Install [Adobe AIR SDK](https://www.adobe.com/devnet/air/air-sdk-download.html)
2. Install [CrossBridge SDK](http://sourceforge.net/projects/crossbridge-community/files/)
3. Set **AIR_HOME** environment variable pointing to the AIR SDK location
4. Set **FLASCC_ROOT** environment variable pointing to the CrossBridge SDK location
5. Add **FLASCC_ROOT/sdk/usr/bin** to the Path (OSX)
6. Run **make** (OSX) or **build.bat** (Windows)

## Using the library

Check the Example(s) located at: src/main/actionscript

You can also see the documentation on LuaState, LuaReference, and LuaEnums for use.

## Running the UnitTests

1. Install [Gradle](http://www.gradle.org/)
2. Run **gradle**
3. Idk if these actually work I haven't run them.
