#!/bin/sh

command -v tmux >/dev/null 2>&1 || (echo "tmux not installed" && exit 1)

. ./Meta/env.sh && \
    make -C Build install && \
    ./Meta/build_grub.sh && \

GDB_COMMAND="gdb Build/Kernel/Kernel \
   -ex 'set debug-file-directory Build/Kernel' \
   -ex 'set arch i386:intel' \
   -ex 'set print asm-demangle on' \
   -ex 'target remote localhost:1234'"

tmux \
    new-session "sudo ./Meta/run.sh qemu -g" \; \
    split-window "$GDB_COMMAND" \; \
    select-pane -U \; \
