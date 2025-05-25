#!/bin/sh

BRIDGE_IP="10.0.2.100"

die() {
  echo "die: $*"
  exit 1
}

if [ "$(id -u)" != 0 ]; then
  exec sudo -E -- "$0" "$@" || die "this script needs to run as root"
else
  : "${SUDO_UID:=0}" "${SUDO_GID:=0}"
fi

ip link delete br0
ip link delete tap0

ip link add br0 type bridge || die "failed to create bridge"
ip addr add ${BRIDGE_IP}/24 dev br0 || die "failed to give bridge an ip"
ip link set br0 up || die "failed to bring bridge up"
echo "br0 -> ${BRIDGE_IP}"

ip tuntap add tap0 mode tap user $(whoami) || die "failed to create tap"
ip link set tap0 up || die "failed to bring tap up"
ip link set tap0 master br0 || die "failed to to tap master"
echo "tap0 -> br0"

echo "done"
