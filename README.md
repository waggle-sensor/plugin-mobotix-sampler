# Mobotix Image & Thermal Sampler Plugin

This plugin utilizes [PyWaggle](https://github.com/waggle-sensor/pywaggle) library to capture & upload frames and thermal data from a Mobotix thermal camera stream.

## Usage

To execute the plugin use the following (from within the built `Docker` container):

```
python3 /app/app.py --ip <Camera IP>
```

Example:

```
python3 /app/app.py --ip 10.11.12.13 --user user1 --password password1 -w /data/files/here/ -f 50 -t 5
```

The above example will capture at-most 50 frames & thermal data over 5 seconds and use the `/data/files/here` director as it's working directory.

*Note*: See the additional command line options via `python3 /app/app.py --help`.

### Direct Mobotix API Access

The API interfacing with the Mobotix camera is done via the `EventStreamClient` SDK (https://developer.mobotix.com/). A custom `thermal-raw` binary application is built to communicate over this interface (from within the built `Docker` container).

```
./thermal-raw --url <Camera IP>
```

Example:

```
./thermal-raw --url 10.11.12.13 --user user1 --password password1 --dir data
```

The above example will continually capture frames & thermal data and store the files in the local `./data` directory.

*Note*: See the additional command line options in `mobotix_sdk/eventstreamclient/plugin-client/thermal-raw/main.cpp`.

### Development

The `Docker` container can be built by executing the following:

```
./build.sh
```
