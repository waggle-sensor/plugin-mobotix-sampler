#!/bin/bash -e

docker build --pull -f Dockerfile.mobotix \
        -t plugin-mobotix:latest .

