#!/usr/bin/env bash
#
# Create an ext2 filesystem
# ./build_fs.sh [directory] [directory to mount to]
set -eu

PATH="$PATH:/sbin"

mkdir -p "$1"

if [ ! -f "$1.img" ]; then
    mke2fs \
      -L '' \
      -N 0 \
      -O ^64bit \
      -d "$1" \
      -m 5 \
      -r 1 \
      -t ext2 \
      "$1.img" \
      32M
fi

mkdir -p "$2"
sudo mount "$1.img" "$2"
