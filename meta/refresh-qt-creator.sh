#!/bin/sh

find . \( -name toolchain \) -prune -o \( -name '*.cpp' -or -name '*.idl' -or -name '*.c' -or -name '*.h' -or -name '*.S' -or -name '*.sh' -or -name 'Makefile' \) -print > os.files
