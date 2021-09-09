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
#include "MxPEG_Audio.hpp"

#include <iostream>
#include <memory>
#include <chrono>

#include <cstring>
#include <cstdio>

#if defined (_MSC_VER)
#include <fcntl.h>
#include <io.h>
#endif

#define URL_SZ 256

using namespace ie::MxPEG;

#define SEND_BUF_SIZE 1024

/*
 * Simple example to show how audio data can be sent to the cameras speaker.
 *
 * Please note: The camera uses a quite small stream buffer for input audio data. If the data arrives too fast, the camera will just
 * drop the excess data. Therefore this example uses a simple mechanism based on the system time to emulate a real time input source.
 *
 * If the source is a real time stream device like a microphone, this mechanism is usually not needed and the data can just be sent
 * as it is read from the device.
 *
 */
int send_audio(std::shared_ptr<MxPEG_SDK> client, char * src, MxPEG_AudioCodec codec, MxPEG_SampleFormat format, int freq, int chan, MxPEG_AudioCodec send_codec) {
   static int audioReturnChannelStatus = 0;
   static FILE * inFile = nullptr;
   static long next = 0;

   if(audioReturnChannelStatus == 0) {
      /*
       * First call: Open the input file and setup the audio session with the camera
       */
      std::cout << "Opening input file " << src << std::endl;
      if(inFile == nullptr) {
         inFile = fopen(src,"r");
         if(inFile == nullptr) {
            perror("Error opening input file.");
            exit(1);
         }
      }
	  
#if defined (_MSC_VER)
      //for windows set the in stream to binary mode
      int res = _setmode(_fileno(inFile), _O_BINARY);
	  std::cout << "changed input mode to binary for " << src << ": " << res << std::endl;
#endif

      std::cout << "Setting up audio return channel using " << ((send_codec==eac_pcm)?"pcm":"aLaw") << " mode." << std::endl;
     /*
      * The SDK only supports sending either pcm16, 1ch, 16kHz or aLaw, 1ch, 8kHz. Therefore the two formats can be indentified by just passing the codec
      */
      client->sendAudioStart(send_codec);
      audioReturnChannelStatus++;

      /*
       * Time when to send next audio package
       */
      next = std::chrono::time_point_cast< std::chrono::microseconds >(std::chrono::system_clock::now()).time_since_epoch().count();
   }  else
   if(audioReturnChannelStatus > 0) {
      /*
       * Check if a new package needs to be sent
       */
      long now = std::chrono::time_point_cast< std::chrono::microseconds >(std::chrono::system_clock::now()).time_since_epoch().count();
      if(inFile && now >= next) {
         MxPEG_AudioFormat fmt(codec,format, freq,chan);

         uint8_t buf[SEND_BUF_SIZE];
         size_t toSendSamples = 0;

         /*
          * Make sure to only read full samples (in case input format uses more than one byte per sample)
          */
         toSendSamples = fread(buf,fmt.bytesPerSample(),SEND_BUF_SIZE/fmt.bytesPerSample(),inFile);

         if(toSendSamples > 0) {
            /*
             * calculate play time of the data loaded from the file
             */
            int buf_duration_us = toSendSamples * fmt.sampleDuration();

            size_t toSendBytes = toSendSamples * fmt.bytesPerSample();
            /*
             * The buffer passed to the send function should always contain full samples
             */
            MxPEG_ReturnCode rc = client->sendAudio(fmt, buf,toSendBytes);

            /*
             * Both return codes indicate
             */
            if(rc == er_Success || rc == er_noData) {
               /*
                * Calculate time when the next package needs to be sent.
                */
               next += buf_duration_us;
               return toSendBytes;
            }

            /*
             * an error occurred - stop sending data
             */
            std::cout << "Failed to send audio: "<< rc << std::endl;
            fclose(inFile);
            std::cout << "Closing audio return channel" << std::endl;
            client->sendAudioStop();
            return -1;
         } else {
            /*
             * Done sending audio data, close session and exit
             */
            std::cout << "No more data to send, closing audio return channel" << std::endl;
            fclose(inFile);
            client->sendAudioStop();
            return -1;
         }
      }
   }
   return 0;
}


int main(int argc, char** argv) {
   std::cout << "EventStream client SDK send audio example" << std::endl;

   char * cam_ip = NULL;
   char * audio_source = NULL;

   if(argc < 3) {
      std::cout << "Missing parameter Hostname/IP" << std::endl;
      std::cout << "Start with " << argv[0] << " [hostname|IP] [pcm-file] [-src-codec <codec>] [-send-codec <codec>]" << std::endl;
      std::cout << "codec  := pcm, alaw" << std::endl;
      std::cout << "This example expects input either as pcm or alaw (= the formats that are created by the live-stream example)."<< std::endl;
      std::cout << "The exact formats are: " << std::endl;
      std::cout << "  PCM, s16 le, 16kHz, 1 channel" << std::endl;
      std::cout << "  a-Law, u8, 8kHz, 1 channel" << std::endl;
      std::cout << "Press enter to exit" << std::endl;
      getchar();
      exit(1);
   }

   cam_ip = argv[1];
   audio_source = argv[2];

   int channels = 1;
   int freq = 16000;
   MxPEG_AudioCodec codec = eac_pcm;
   MxPEG_AudioCodec send_codec = eac_pcm;
   MxPEG_SampleFormat format = easf_S16;

   int i = 2;
   while (i < argc) {
      if ((strcmp(argv[i], "-src-codec") == 0) && (i + 1 < argc)) {
         if( strcmp("pcm",argv[i+1]) == 0) {
            codec = eac_pcm;
            //for pcm switch to 16kHz, s16
            freq = 16000;
            format = easf_S16;
         } else if( strcmp("alaw",argv[i+1]) == 0) {
            codec = eac_alaw;
            //for alaw switch to 8kHz, u8
            freq = 8000;
            format = easf_U8;
         } else {
            std::cout << "-src-codec: Unknown audio codec " << argv[i+1] << std::endl;
         }
         i += 2;
      } else 
      if ((strcmp(argv[i], "-send-codec") == 0) && (i + 1 < argc)) {
         if( strcmp("pcm",argv[i+1]) == 0) {
            send_codec = eac_pcm;
         } else if( strcmp("alaw",argv[i+1]) == 0) {
            send_codec = eac_alaw;
         } else {
            std::cout << "-send-codec: Unknown audio codec " << argv[i+1] << std::endl;
         }
         i += 2;
      }
      else {
         i++;
      }
   }

   std::cout << "Audio input format: codec=" << codec << " format=" << format << " freq=" << freq << " channels=" << channels << std::endl;
   std::cout << "Audio output/transfer format: codec " << send_codec << std::endl;
   
   char url[URL_SZ] = "";
   snprintf(url,URL_SZ,"http://%s/control/eventstream.jpg",cam_ip);


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
    * Deactivate audio stream from the camera
    */
   client->setAudioActive(false);

   /*
    * Deativate video for this session
    */
   client->setVideoActive(false);

   /*
    * tell the camera to start sending live pictures
    */
//   client->startLive();


   /*
    * The SDK library is single threaded, the function loop() acts as the main loop of the library. Call it repeatedly
    * from the applications main loop.
    */
   while(true) {
      client->loop();
      if(send_audio(client, audio_source, codec, format, freq, channels, send_codec) < 0) {
         //stop on error or if no more audio data is available from input
         break;
      }
   }

#ifdef _MSC_VER
   WSACleanup();
#endif
}
