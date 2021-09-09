EventStream Client SDK
======================

The SDK contains pre-compiled libraries to support communication with the EventStream service of a MOBOTIX camera.

The EventStream service allows you to receive live video and audio streams, to access and play recordings and to use remote PTZ.

The included sample code shows basic usage of the SDK.

Contents of the package
=======================

doc -> Documentation folder
include -> Include files needed to use the library
lib/Linux -> Linux (x86_64) version of the library (Linux package only)
lib/Darwin -> OS X (x86_64) version of the library (OSX package only)
lib/windows -> Windows (x86_64/x86) version of the library (Windows package only)
samples/live-stream -> Sample client that shows basic usage of the library
samples/send-audio -> Sample application that shows how audio data can be sent to the camera
samples/player -> Sample player that shows more advanced usage of the library (requires SDL2)
samples/thermal-raw -> Sample client that fetches thermal raw data. It also contains examples to show how to process the raw data.
samples/decoder -> Sample that shows how to use the decoder-only module to decode MxPEG frames into raw data (can be used to 
	extract thermal-raw data from MxPEG frames)

To use the EventStream client library you need to implement 4 - 5 interfaces:

1) MxPEG_AuthorizationHandler
=============================

This mainly provides the means to provide access to user name and password to the
library. 


2) MxPEG_EventNotificationListener
==================================

All messages received from the camera will be passed to this listener. Each message
is associated with an id that was included in the request that caused the message.


3) MxPEG_SinkAudio
==================

Receives all audio data from the camera. The format of the data provided by the
library is always PCM16, 1 channel, 16kHz. If the camera sends a different format
like a-law, the audio data is converted internally before it is passed to the sink.

4) MxPEG_SinkVideo
==================

Receives all video data from the camera. The video format is raw YUV, Planar 420

All audio and video buffers handed to the sink objects contain time stamps to allow
the client to implement A/V synced playback.

The sample client in the folder "sample" shows how to connect to a camera and receive the live video and audio streams stream.

5) MxPEG_NetworkConnection (optional)
=====================================

By implementing this interface you can take control over the low level network operations of the SDK. You can use this for example to implement 
SSL encryption for the network communication. The "player" example includes a sample implementation of this interface.

Samples
=======

To build the examples you'll need gnu make and gcc (Linux and OSX) or Visual Studio 2015 (Windows). Additionally for example 2, you'll need the SDL2 library and header files.

The examples included in this SDK package use an authorization handler that simply returns the MOBOTIX default
credentials of the admin account:

User: admin
Password: meinsm

If your test camera requires a different set of credentials, you need to modify the AuthorizationHandler.cpp in the according sample folder.


1) live-stream
--------------

This is a simple example application that shows how to fetch live video and audio stream from the camera. The received stream is decoded and stored as raw data.

To build the example for Linux and OSX:
---------------------------------------

- Open a console and navigate to the folder "samples/live-stream". 
- Run "make"

The make command will create a folder name "build" and place all output into that folder. 

To run the example:

./build/live-stream <CAMERA-IP>

This example client will store raw video and audio data in two separate output files: video_stream.raw and audio_stream.raw

The video file contains frames in raw YUV Planar 4:2:0 format. You can play it with:

vlc --demux rawvideo --rawvid-fps 12 --rawvid-width $WIDTH --rawvid-height $HEIGHT --rawvid-chroma I420 video_stream.raw

Please remember to specify the width and height as reported by the live-stream demo.

The audio file contains 1 channel of raw PCM16 little endian audio samples with a sample rate of 16000. To play it use:

vlc --demux=rawaud --rawaud-channels 1 --rawaud-samplerate 16000 audio_stream.raw

Please note: When using vlc as test player on OSX you need to specify the full path to the VLC executable (not the application bundle). For example, 
if vlc is installed to the folder /Applications, then replace the string „vlc“ in the above examples with „/Applications/VLC.app/Contents/MacOS/VLC“.


To build the example for Windows:
---------------------------------

Open the solution file "samples\live-stream\vc2015\live-stream.sln" in Visual Studio (tested with version 2015) and select platform and target
and then build the Project from the VS menu.

Depending on the selected platform and target the executable will be located in one of these folders:

"Debug" 
"Release" 
"x64/Debug"
"x64/Release" 

To run the executable use a command shell and change into the according target folder and run:

live-stream.exe <CAMERA-IP>

The generated output files have the same format as the ones described in the Linux/OSX section and you use vlc in the same way to play the files.


2) player
---------

This example shows the usage of basic playback controls, it shows how to use camera side PTZ and it contains example code that deonstrates how to use your own custom network layer (for example to add SLL support).

Linux:
------
Make sure to install the SDL2 library and devel packages on your system. The build system uses the tool "sdl2-config" to acquire all necessary information. Make sure that this tool is located in your default path before running make.

OSX:
----
The build system requires the SDL2 framework package. You can download that from

https://www.libsdl.org/download-2.0.php

Choose SDL2-2.x.y.dmg in the "Development Libraries" section.

The default location as recommended in the Read-Me file of the SDL2 package is

/Library/Frameworks/SDL2.framework

Just copy the SDL2 framework package to the above folder as described in the ReadMe.txt file of the SDL2 dmg. 
 
Windows:
--------

Download the SDL2-devel package for Visual C++ from

https://www.libsdl.org/download-2.0.php

To install the package follow these steps:

1) Unzip the package to the solution folder (samples\player\vc2015). You will get a folder named like this "SDL2-2.x.y" 
2) Rename that folder to "SDL2"

The build system checks for the SDL2 include and library files. If you get an error message stating that SDL2 was not found, double 
check the SDL2 installation. The SDL2 header files should be located in the directory 

samples\player\vc2015\SDL2\include

And the SDL2 library files should be located in the directories

samples\player\vc2015\SDL2\lib\x86
samples\player\vc2015\SDL2\lib\x64


To build the example (Linux/OSX):
---------------------------------

- Open a console and navigate to the folder "samples/player". 
- Run "make"

The make command will create a folder name "build" and place all output into that folder. 

To run the example:

cd build
./eventstream-player <CAMERA-IP>

The player will open a window to display the live video stream from the camera. To control the application use the following hot-keys:

Stream controls
Switch between player mode and live stream

l       Start live playback (default at startup)
p       Play recording from current position
space   Pause stream (use p or l to resume stream)

Player controls
While playing clips you can change the playback speed. Negative speed will
reverse playback. Setting speed to zero pauses the stream. You can use
the cursor keys to navigate through the events.

1       Set playback speed to 1
+       Increase playback speed by 1 
-       Decrease playback speed by 1
Left    Jump to previous event
Right   Jump to next event
Up      Jump to head (most recent) event
Down    Jump to tail (oldest) event

PTZ
Use the mouse to change image center and to zoom

Left    Change image center
Wheel   Zoom in/out

Misc
h       Show this message
q       Exit application

To build the example for Windows:
---------------------------------

Open the solution file "samples\player\vc2015\player.sln" in Visual Studio (tested with version 2015) and select platform and target
and then build the Project from the VS menu.

Depending on the selected platform and target the executable will be located in one of these folders:

"Debug" 
"Release" 
"x64/Debug"
"x64/Release" 

To run the executable use a command shell and change into the according target folder and run:

player.exe <CAMERA-IP>

The mouse and keyboard controls are the same as for the Linux/OSX version.

3) send-audio
--------------

This is a simple example application that shows how to send audio data back to the camera.

To build the example for Linux and OSX:
---------------------------------------

- Open a console and navigate to the folder "samples/send-audio". 
- Run "make"

The make command will create a folder name "build" and place all output into that folder. 

To run the example:

./build/send-audio <CAMERA-IP> <PCM-File>

The PCM file needs to be PCM with a sample rate of 16000 Hz using signed 16 bit samples (le) with 1 channel. You can just use one of the audio files generated by the live-stream sample application.

To build the example for Windows:
---------------------------------

Open the solution file "samples\send-audio\vc2015\send-audio.sln" in Visual Studio (tested with version 2015) and select platform and target
and then build the Project from the VS menu.

Depending on the selected platform and target the executable will be located in one of these folders:

"Debug" 
"Release" 
"x64/Debug"
"x64/Release" 

To run the executable use a command shell and change into the according target folder and run:

send-audio.exe <CAMERA-IP> <PCM-File>

4) thermal-raw
--------------

This sample application shows to to request and receive thermal raw data from a camera with thermal sensor. 

For this example you will need a camera with thermal sensor. Additionally raw thermal data export needs to be activated in the camera configuration. To do this open the "Thermal Sensor Settings" in the setup interface of the camera. In the section "Thermal Raw Data" select "Enabled". If the camera has a TR type sensor you can also enable "Linear Mode" in the settings menu, then the SDK will also offer to convert the raw thermal data to absolute temeratures.

To build the example for Linux and OSX:
---------------------------------------

- Open a console and navigate to the folder "samples/thermal-raw". 
- Run "make"

The make command will create a folder name "build" and place all output into that folder. 

To run the example:

./build/thermal-raw <CAMERA-IP>

The sample application will output several files for each received frame:

1) The decoded RGB frame of the video stream
2) The thermal raw data stored as binary binary output as it is received from the camera (if thermal raw export is activated)
3) Thermal data converted to numeric values. The data is stored as integer values in a csv file.
4) Thermal data converted to degrees Celsius. The data is stored as float values in a csv file. (only for TR sensors)


To build the example for Windows:
---------------------------------

Open the solution file "samples\thermal-raw\vc2015\thermal-raw.sln" in Visual Studio (tested with version 2015) and select platform and target
and then build the Project from the VS menu.

Depending on the selected platform and target the executable will be located in one of these folders:

"Debug" 
"Release" 
"x64/Debug"
"x64/Release" 

To run the executable use a command shell and change into the according target folder and run:

thermal-raw.exe <CAMERA-IP>

The generated output files have the same format as the ones described in the Linux/OSX section.


4) decoder
--------------

This sample application shows how to decode a single MxPEG frame using the SDKs decoder API. The decodeder will also extract thermal raw data - if available.

To build the example for Linux and OSX:
---------------------------------------

- Open a console and navigate to the folder "samples/decoder". 
- Run "make"

The make command will create a folder name "build" and place all output into that folder. 

To run the example:

./build/decoder <INPUT-FRAME>

The sample application will decode the input frame. Depending on the contents of the input frame the application will create one or more 
files:

1) The decoded RGB image

If thermal raw data is included in the input frame:

2) The thermal raw data stored as binary binary output as it is received from the camera (if thermal raw export is activated)
3) Thermal data converted to numeric values. The data is stored as integer values in a csv file.
4) Thermal data converted to degrees Celsius. The data is stored as float values in a csv file. (only for TR sensors)


To build the example for Windows:
---------------------------------

Open the solution file "samples\thermal-raw\vc2015\decoder.sln" in Visual Studio (tested with version 2015) and select platform and target
and then build the Project from the VS menu.

Depending on the selected platform and target the executable will be located in one of these folders:

"Debug" 
"Release" 
"x64/Debug"
"x64/Release" 

To run the executable use a command shell and change into the according target folder and run:

decoder.exe <INPUT-FRAME>

The generated output files have the same format as the ones described in the Linux/OSX section.

