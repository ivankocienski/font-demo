# font-demo

A font rendering library for games. Will load a True Type font and draw a
palette of characters where by you can screen grab and touch up the font
for use in games. Also draws a set of strings. Sizes can be adjusted on
the fly.

## Requirements

- cmake
- C++ 11
- SDL
- libfreetype

## To build

    mkdir font-demo
    
    git clone <bla> ~/font-demo/repo

    cd ~/font-demo

    mkdir build

    cd build

    cmake ../repo/

    make

    ./font-demo


## Basic usage

Here is a quick example

    ./font-demo --size 50 --strings data/strings.txt data/large.ttf

Press `TAB` to switch to strings mode. Press `PAGE-UP` and `PAGE-DOWN`
to adjust font sizes on the fly