//           ///          //                                  C++ Cross Platform
//          /////        ////
//         /// XXX     XXX ///            ///////////   /////////     ///   ///
//        ///    XXX XXX    ///         ///             ///    ///   ///  ///
//       ///       XXX       ///         /////////     ///      //  //////
//      ///      XXX XXX      ///               ///   ///    ///   ///  ///
//     ////    XXX     XXX    ////    ////////////  //////////    ///   ///
//    ////                     ////
//   ////  M  O  B  O  T  I  X  ////////////////////////////////////////////////
//  //// Security Vision Systems //////////////////////////////////////////////
//
//
///////
     //
     //  MOBOTIX EventStream Client SDK 
     //
     //  This software is licensed under the BSD license,
     //  http://www.opensource.org/licenses/bsd-license.php
     //
     //  Copyright (c) 2005 - 2016, MOBOTIX AG
     //  All rights reserved.
     //
     //  Redistribution and use in source and binary forms, with or without
     //  modification, are permitted provided that the following conditions are
     //  met:
     //
     //  - Redistributions of source code must retain the above copyright
     //    notice, this list of conditions and the following disclaimer.
     //
     //  - Redistributions in binary form must reproduce the above copyright
     //    notice, this list of conditions and the following disclaimer in the
     //    documentation and/or other materials provided with the distribution.
     //
     //  - Neither the name of MOBOTIX AG nor the names of its contributors may
     //    be used to endorse or promote products derived from this software
     //    without specific prior written permission.
     //
     //  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
     //  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
     //  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
     //  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
     //  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
     //  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
     //  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
     //  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
     //  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
     //  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
     //  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
     //
///////

/*
 *
 * This is a simple example application that shows how to fetch live video and  thermal raw data from the camera. The received
 * stream is decoded and stored as raw data in separate file sets for each frame.
 *
 * Video is stored as stream_FFFFFF_XXXXxYYYY.rgb
 * FFFFFF = frame number
 * XXXX = width of the rgb frame
 * YYYY = height of the rgb frame
 *
 * Thermal raw data is stored as
 * stream_FFFFFF_SENSOR_XXXXxYYYY_BBB.thermal.raw
 * FFFFFF = frame number
 * SENSOR = sensor number of the raw sensor (either 1 or 2)
 * XXXX = width of the thermal raw image
 * YYYY = height of thermal raw image
 * BBB = bit depth (current sensors always use 14 bit)
 *
 * To view those raw files you'll need a special raw image viewer (for example: vooya http://www.offminor.de/downloads.html)
 *
 */

#include "MxPEG_SDK_API.hpp"

#include "SinkVideo.h"
#include "SinkAudio.h"
#include "AuthorizationHandler.h"
#include "NotificationListener.h"

#include <iostream>
#include <memory>

#define URL_SZ 256

using namespace ie::MxPEG;

int main(int argc, char** argv) {
   std::cout << "EventStream client SDK thermal raw data example" << std::endl;

   if(argc < 2) {
	   std::cout << "Missing parameter hostname/IP" << std::endl;
	   std::cout << "Start with " << argv[0] << " [hostname|IP]" << std::endl;
	   std::cout << "Press enter to exit" << std::endl;
	   getchar();
	   exit(1);
   }

   char url[URL_SZ] = "";
   snprintf(url,URL_SZ,"http://%s/control/eventstream.jpg",argv[1]);

#ifdef _MSC_VER
   WSADATA wsaData;
   int iResult;
   iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
   if (iResult != NO_ERROR) {
	   std::cout << "WSAStartup failed: " << iResult << std::endl;
	   return 1;
   }
#endif

   /*
    * Create a video sink object to process the decoded video frames
    */
   MxPEG_SinkVideo::shared_ptr_t sinkVideo = MxPEG_SinkVideo::shared_ptr_t(new SinkVideo("stream"));

   /*
    * Create an audio sink object to process the received audio packages
    */
   MxPEG_SinkAudio::shared_ptr_t sinkAudio = MxPEG_SinkAudio::shared_ptr_t(new SinkAudio());

   /*
    * Create an authorization handler that provides the credentials (if needed)
    */
   MxPEG_AuthorizationHandler::shared_ptr_t authHandler = MxPEG_AuthorizationHandler::shared_ptr_t(new AuthorizationHandler());

   /*
    * Create a notification listener to handle the json replies of the camera
    */
   MxPEG_EventNotificationListener::shared_ptr_t notificationListener = MxPEG_EventNotificationListener::shared_ptr_t(new NotificationListener());

   /*
    * Create a new eventstream client SDK object
    */
   MxPEG_SDK::shared_ptr_t client = MxPEG_SDK_Factory::create(sinkAudio, sinkVideo, MxPEG_ImageMode::im_RGB, authHandler,notificationListener);

   /*
    * configure the client to connect to camera with hostname "i25"
    */
   client->stream_setup(url);

   /*
    * Do not activate audio for this session (the audio sink in this example will drop all audio data anyway)
    */
   client->setAudioActive(false);

   /*
    * activate video for this session
    */
   client->setVideoActive(true);

   /*
    * activate raw thermal data export for this session
    *
    * -> To use this feature the camera must be equipped with at least one thermal sensor that is configured
    * for raw export. (Open the camera setup menu and navigate to: Setup->Thermal Sensor Settings, set the option
    * "Thermal Raw Data" to "enabled".
    *
    * See SinkVideo.[cpp|h] for examples how to access and process the thermal raw data
    */
   client->setThermalRawExportActive(true);

   /*
    * tell the camera to start sending live pictures (thermal raw data is sent as part of the video stream)
    */
   client->startLive();

   /*
    * The SDK library is single threaded, the function loop() acts as the main loop of the library. Call it repeatedly
    * from the applications main loop.
    */
   while(true) {
      client->loop();
   }

#ifdef _MSC_VER
   WSACleanup();
#endif
}
