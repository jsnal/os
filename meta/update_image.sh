#!/usr/bin/env bash

PATH="$PATH:/sbin"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/.."

sudo losetup /dev/loop0 $DIR/toolchain/a.img
sudo mount /dev/loop0 /mnt
sudo cp $DIR/kernel/kernel.bin /mnt/kernel
sudo umount /dev/loop0
sudo losetup -d /dev/loop0
