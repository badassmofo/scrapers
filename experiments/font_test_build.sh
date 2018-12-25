#!/bin/sh
clang font_test_$1.c font_test_helper.c 3rdparty/glad.c -framework OpenGL -lSDL2 -lFreetype -I/usr/local/include -L/usr/local/lib -Ifreetype
./a.out
rm a.out
