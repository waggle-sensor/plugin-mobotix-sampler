#!/bin/bash -e

docker build --pull -f Dockerfile.mobotix \
        -t plugin-mobotix:latest .

# docker build --pull -f Dockerfile.arm64.mobotix \
#         -t plugin-mobotix:arm64 .