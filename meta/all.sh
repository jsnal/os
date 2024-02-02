#!/bin/sh

make -C kernel
./meta/build_grub.sh
./meta/run.sh $@
