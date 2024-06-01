#!/bin/sh

PATH="$PATH:/sbin"
DIR=$(dirname $(cd -P -- "$(dirname -- "$0")" && pwd -P))

echo "building filesystem..."

cp -Rv $DIR/Base/* mnt/
chown -R 100:100 mnt/home/user

mkdir -p mnt/dev
mknod -m 666 mnt/dev/random c 1 9

echo "copying user programs..."
mkdir -p mnt/bin
cp -Rv $DIR/Programs/bin/* mnt/bin/

echo "done"

