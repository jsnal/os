#!/bin/sh

. ./Meta/env.sh && \
    make -C Build/Tests && \
    ./Build/Tests/$1
