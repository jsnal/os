#!/bin/sh

PATH="$PATH:/sbin"
DIR=$(dirname $(cd -P -- "$(dirname -- "$0")" && pwd -P))

echo "building filesystem..."

cp -Rv $DIR/Base/* mnt/
chown -R 100:100 mnt/home/user

echo "copying user programs..."
mkdir -p mnt/bin
cp -Rv $DIR/Programs/bin/* mnt/bin/

echo "done"

