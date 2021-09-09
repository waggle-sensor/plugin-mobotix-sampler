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
 * This is a simple example application that shows how to fetch live video and audio from the camera. The received
 * stream is decoded and stored as raw data in separate files for video and audio.
 *
 * To play those raw files you can use vlc:
 *
 * Audio:
 *
 * vlc --demux=rawaud --rawaud-channels 1 --rawaud-samplerate 16000 audio_stream.raw
 *
 * Video:
 *
 * vlc --demux rawvideo --rawvid-fps 12 --rawvid-width <WIDTH> --rawvid-height <HEIGHT> --rawvid-chroma I420 video_stream.raw
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
   std::cout << "EventStream client SDK live stream example" << std::endl;

   if(argc < 2) {
	   std::cout << "Missing parameter Hostname/IP" << std::endl;
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
   MxPEG_SinkVideo::shared_ptr_t sinkVideo = MxPEG_SinkVideo::shared_ptr_t(new SinkVideo("video_stream.raw"));

   /*
    * Create an audio sink object to process the received audio packages
    */
   MxPEG_SinkAudio::shared_ptr_t sinkAudio = MxPEG_SinkAudio::shared_ptr_t(new SinkAudio("audio_stream.raw"));

   /*
    * Create an authorization handler that provides the credentials (if needed)
    */
   MxPEG_AuthorizationHandler::shared_ptr_t authHandler = MxPEG_AuthorizationHandler::shared_ptr_t(new AuthorizationHandler());

   /*
    * Create a notification listener to handle the json replys of the camera
    */
   MxPEG_EventNotificationListener::shared_ptr_t notificationListener = MxPEG_EventNotificationListener::shared_ptr_t(new NotificationListener());

   /*
    * Create a new eventstream client object
    */
   MxPEG_SDK::shared_ptr_t client = MxPEG_SDK_Factory::create(sinkAudio, sinkVideo, MxPEG_ImageMode::im_YUV, authHandler,notificationListener);

   /*
    * configure the client to connect to camera with hostname "i25"
    */
   client->stream_setup(url);

   /*
    * activate audio for this session (only has an effect if the camera has a microphone that is activated)
    */
   client->setAudioActive(true);

   /*
    * ativate video for this session
    */
   client->setVideoActive(true);

   /*
    * tell the camera to start sending live pictures
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
