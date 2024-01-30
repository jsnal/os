#!/bin/sh

DIR=$(dirname $(cd -P -- "$(dirname -- "$0")" && pwd -P))
PATH="$PATH:$DIR/toolchain/local/bin"

die() {
  echo "die: $*"
  exit 1
}

if [ "$(id -u)" != 0 ]; then
  exec sudo -E -- "$0" "$@" || die "this script needs to run as root"
else
  : "${SUDO_UID:=0}" "${SUDO_GID:=0}"
fi

extra_arguments=""

if [ "$1" = "monitor" ]; then
    echo 'enabling qemu monitor...'
    extra_arguments="-monitor tcp:127.0.0.1:55555,server,nowait"
fi

if [ "$1" = "gdb" ]; then
    echo 'enabling gdb...'
    extra_arguments="-s -S"
fi

make -C "$DIR/kernel" || die "make failed"
"$DIR/meta/build_grub.sh"
$DIR/toolchain/qemu-*/qemu-system-i386 $extra_arguments \
  -serial stdio -drive file=os_grub.img,format=raw,index=0,media=disk \
  || die "unable to run qemu"
