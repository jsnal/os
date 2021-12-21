#!/bin/sh

DIR=$(dirname $(cd -P -- "$(dirname -- "$0")" && pwd -P))

sudo $DIR/toolchain/bochs-*/bochs -f $DIR/meta/bochsrc.txt -q
