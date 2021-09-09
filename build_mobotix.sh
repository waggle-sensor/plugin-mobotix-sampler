#!/bin/bash -e

# todo more stuff here.. parameters?

docker build --pull -f Dockerfile.mobotix \
        -t plugin-mobotix:latest .

docker run --rm plugin-mobotix:latest
