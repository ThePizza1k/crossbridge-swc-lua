crossbridge-patches
===================

Patches to fix memory safety issues in CModule.

## Fixes
CModule.mallocString would write data even if the allocation failed. It now throws an error if malloc returns null.

## Building the patches

1. First, set up your Crossbridge build environment. (See **../README.md**)
2. In the crossbridge environment, navigate to this directory and run **make**. It will build abc files and place them in this directory.

## Installing the patches.
1. Put all of the abc files into **FLASCC_ROOT/sdk/usr/lib**, overwriting the old files.
