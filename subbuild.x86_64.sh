#!/bin/bash

echo "Building x86 specific steps"

apt-get update && apt-get install -y \
    gcc \
    g++
