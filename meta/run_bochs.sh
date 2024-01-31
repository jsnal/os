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

make -C "$DIR/kernel" || die "make failed"
"$DIR/meta/build_grub.sh"
$DIR/toolchain/bochs-*/bochs-*/bochs -f $DIR/meta/bochsrc.txt -q || die "unable to run bochs"
