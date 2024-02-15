#!/usr/bin/env bash

set -eo pipefail

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

PREFIX="$DIR/local"

QEMU_VERSION="6.0.1"
QEMU_NAME="qemu-$QEMU_VERSION"
QEMU_PKG="${QEMU_NAME}.tar.xz"
QEMU_BASE_URL="https://download.qemu.org"

if [ ! -d ${QEMU_NAME} ]; then
  mkdir $QEMU_NAME
  pushd $QEMU_NAME
    echo "Downloading qemu"
    curl -LO "$QEMU_BASE_URL/$QEMU_PKG"

    echo "Extracting qemu"
    tar -xf ${QEMU_PKG}

    ${QEMU_NAME}/configure --prefix="$PREFIX" --target-list=i386-softmmu --disable-vnc --enable-sdl
    make -j
  popd
fi
