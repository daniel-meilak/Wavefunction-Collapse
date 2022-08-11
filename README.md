# Wavefunction Collapse using C++ and raylib

This repo is my attempt at implementing a simple wavefunction collapse algorithm specifically for tilesets. It is inspired by the work of [Maxim Gumin](https://github.com/mxgmn/WaveFunctionCollapse).

Currently the work requires raylib to be preinstalled (with a working C++ compiler) to run. In the future I will include a playable version (compiled with ecmascript) on Itch.io and possibly add a CMake build.

The C++ code is written using modern C++, and requires C++20 support to compile. 

The circuit tileset included in `tilesets/` is also from Maxim Gumin's [repo](https://github.com/mxgmn/WaveFunctionCollapse/tree/master/tilesets/Circuit) (with minor adjustments). The data file has no specific format, I may change it to json/xml/yaml in the future to improve readability.

## Examples:

<p align="center"><img alt="Circuit building animation" src="images/Animation.gif"></p>