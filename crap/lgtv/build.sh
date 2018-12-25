#!/bin/sh
clang lgtv_osx_mk.m -framework Cocoa -framework Foundation -framework Carbon $@ -o lgtv_osx_mk
