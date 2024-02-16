#!/bin/sh

DIR=$(dirname $(cd -P -- "$(dirname -- "$0")" && pwd -P))
extra_arguments=""

die() {
  echo "die: $*"
  exit 1
}

if [ "$(id -u)" != 0 ]; then
  exec sudo -E -- "$0" "$@" || die "this script needs to run as root"
else
  : "${SUDO_UID:=0}" "${SUDO_GID:=0}"
fi

. "$DIR/Meta/env.sh"

if [ "$1" = "bochs" ]; then
    bochs -f $DIR/Meta/bochsrc.txt -q || die "unable to run bochs"
else
    if [ "$1" = "monitor" ]; then
        echo 'enabling qemu monitor...'
        extra_arguments="-monitor tcp:127.0.0.1:55555,server,nowait"
    elif [ "$1" = "gdb" ]; then
        echo 'enabling gdb...'
        extra_arguments="-s -S"

    fi

    qemu-system-i386 $extra_arguments \
        -m 128M \
        -serial stdio \
        -drive file=$DIR/os_grub.img,format=raw,index=0,media=disk \
        || die "unable to run qemu"
fi
