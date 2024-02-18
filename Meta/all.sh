#!/bin/sh

. ./Meta/env.sh && \
    make -C Build install && \
    ./Meta/build_grub.sh && \
    ./Meta/run.sh $@
