#!/bin/bash -e

docker build --pull -f Dockerfile.x86.mobotix \
        -t plugin-mobotix:x86 .
