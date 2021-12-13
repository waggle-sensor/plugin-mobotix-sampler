#!/bin/bash

echo "Building aarch64 specific steps"

TOOLCHAIN_URL=https://github.com/waggle-sensor/nvidia_toolchain/raw/415af777fdac2d6f78d65b908c6aeb28f1c95368/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu.tar.xz
TOOLCHAIN_MD5=6ec789d642584a01e240ab3366599dbb
TOOLCHAIN_DIR=/opt/linaro/

# apt-get update && apt-get install -y \
#     wget \
#     xz-utils

mkdir -p ${TOOLCHAIN_DIR}
wget -c ${TOOLCHAIN_URL}
if ! md5sum $(basename ${TOOLCHAIN_URL}) | grep -q ${TOOLCHAIN_MD5}; then
    echo "Error: toolchain checksum did not match (${TOOLCHAIN_MD5})"
    exit 1
fi
pushd ${TOOLCHAIN_DIR}
tar xf /$(basename ${TOOLCHAIN_URL})
popd
