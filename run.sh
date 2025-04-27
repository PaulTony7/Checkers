#! /bin/bash
gcc -std=c99 main.c -o main -g $(pkg-config --cflags --libs sdl3 sdl3-ttf sdl3-image)
