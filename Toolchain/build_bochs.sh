#!/usr/bin/env bash

set -eo pipefail

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

PREFIX="$DIR/local"

BOCHS_VERSION="20211104"
BOCHS_NAME="bochs-$BOCHS_VERSION"
BOCHS_PKG="${BOCHS_NAME}.tar.gz"
BOCHS_BASE_URL="https://bochs.sourceforge.io/svn-snapshot"

if [ ! -d ${BOCHS_NAME} ]; then
  mkdir $BOCHS_NAME
  pushd $BOCHS_NAME
    echo "Downloading bochs"
    curl -LO "$BOCHS_BASE_URL/$BOCHS_PKG"

    echo "Extracting bochs"
    tar -xvzf ${BOCHS_PKG}

    ${BOCHS_NAME}/configure --prefix="$PREFIX" \
      --enable-smp \
      --enable-cpu-level=6 \
      --enable-all-optimizations \
      --enable-pci \
      --enable-vmx \
      --enable-debugger \
      --enable-disasm \
      --enable-debugger-gui \
      --enable-logging \
      --enable-fpu \
      --enable-3dnow \
      --enable-sb16=dummy \
      --enable-cdrom \
      --enable-x86-debugger \
      --enable-iodebug \
      --enable-e1000 \
      --disable-plugins \
      --disable-docbook \
      --with-x --with-x11 --with-sdl2
    make -j
  popd
fi
