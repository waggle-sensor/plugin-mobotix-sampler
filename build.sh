#!/bin/bash -e

docker build --pull -f Dockerfile \
        -t plugin-mobotix:latest .
