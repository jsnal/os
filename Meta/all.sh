#!/bin/sh

. ./Meta/env.sh && \
    make -C LibC && \
    make -C Kernel && \
    ./Meta/build_grub.sh && \
    ./Meta/run.sh $@
