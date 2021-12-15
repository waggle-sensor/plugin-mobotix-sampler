# This is the build environment to produce the mobotix binary(s)
FROM ubuntu:focal-20211006 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    gcc \
    g++ \
    make \
    libboost-all-dev

ADD mobotix_sdk /build
RUN cd /build/eventstreamclient/lib/Linux && \
    ln -s libeventstreamclient.$(uname -m).a libeventstreamclient.a

RUN /usr/bin/make -C /build/eventstreamclient/plugin-client/thermal-raw

# Running docker image
FROM waggle/plugin-base:1.1.1-base

COPY requirements.txt /app/
RUN pip3 install --no-cache-dir --upgrade -r /app/requirements.txt

# install library dependencies
RUN apt-get update && apt-get install -y \
    libboost-program-options1.71.0 \
    ffmpeg

COPY --from=builder /build/eventstreamclient/plugin-client/thermal-raw/build/thermal-raw /thermal-raw

COPY app /app/

ENTRYPOINT ["python3", "/app/app.py"]
