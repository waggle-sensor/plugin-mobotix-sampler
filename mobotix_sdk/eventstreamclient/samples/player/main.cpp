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
 * This example shows the usage of basic playback controls. It also shows how to use camera side PTZ.
 *
 * To build this example you need the SDL2 library and header files.
 *
 */
#include "MxPEG_SDK_API.hpp"

#include "SinkVideo.h"
#include "SinkAudio.h"
#include "AuthorizationHandler.h"
#include "NotificationListener.h"
#include "NetworkConnection.h"

#include <iostream>

#ifndef _MSC_VER
#include <unistd.h>
#endif

#include <SDL.h>

using namespace ie::MxPEG;

MxPEG_SDK::shared_ptr_t g_client;

bool g_active = true;

#ifdef ASYNC_NETWORK
#include <thread>
#include <mutex>
#include <atomic>

   std::mutex g_async_mutex;
#endif


void help() {
    std::cout << "EventStream demo player\n" << std::endl;
    std::cout << "This player can show either the current live video stream or play back recorded clips from the camera" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "To control the application use the following hot-keys:" << std::endl;

    std::cout << "\nStream controls" << std::endl;
    std::cout << "Switch between player mode and live stream\n" << std::endl;
    std::cout << "l\tStart live playback (default at startup)" << std::endl;
    std::cout << "p\tPlay recording from current position" << std::endl;
    std::cout << "space\tPause stream (use p or l to resume stream)" << std::endl;
    std::cout << "w\tSkip through stream frame by frame using show command (use p or l to resume stream)" << std::endl;
    
    std::cout << "\nPlayer controls" << std::endl;
    std::cout << "While playing clips you can change the playback speed. Negative speed will" << std::endl;
    std::cout << "reverse playback. Setting speed to zero pauses the stream. You can" << std::endl;
    std::cout << "the cursor keys to navigate through the events.\n" << std::endl;
    std::cout << "1\tSet playback speed to 1" << std::endl;
    std::cout << "+\tIncrease playback speed by 1 " << std::endl;
    std::cout << "-\tDecrese playback speed by 1" << std::endl;
    std::cout << "Left\tJump to previous event" << std::endl;
    std::cout << "Right\tJump to next event" << std::endl;
    std::cout << "Up\tJump to head (most recent) event" << std::endl;
    std::cout << "Down\tJump to tail (oldest) event" << std::endl;
    std::cout << "o\tSeeking to timestamp (hardcoded, see source code)" << std::endl;
 
    std::cout << "\nAlarm" << std::endl;
    std::cout << "a\tFetch most recent 10 Alarm events from camera" << std::endl;
    std::cout << "u\tToggle subscription for alarm events from camera" << std::endl;

    std::cout << "\nPTZ" << std::endl;
    std::cout << "Use the mouse to change image center and to zoom\n" << std::endl;
    std::cout << "Left\tChange image center" << std::endl;
    std::cout << "Wheel\tZoom in/out" << std::endl;

    std::cout << "\nAudio" << std::endl;
    std::cout << "kp +\tActivate audio" << std::endl;
    std::cout << "kp -\tDeactivate audio" << std::endl;
    std::cout << "kp 1\tRequest pcm16" << std::endl;
    std::cout << "kp 2\tRequest alaw" << std::endl;

    std::cout << "\nMisc" << std::endl;
    std::cout << "n\tSend ping message to camera" << std::endl;
    std::cout << "h\tShow this message" << std::endl;
    std::cout << "q\tExit application" << std::endl;

}

void poll_sdl_events() {
    static uint32_t last_x = 0;
    static uint32_t last_y = 0;
    static float rate = 1.0;
    
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
    switch ( event.type ) {
        case SDL_QUIT:
        {
            std::cout << "Got SDL_QUIT" << std::endl;
            g_active = false;
            break;
        }
        case SDL_KEYUP:
        {
            SDL_KeyboardEvent * ke = reinterpret_cast<SDL_KeyboardEvent *>(&event);
            switch(ke->keysym.sym) {
                case SDLK_q: 
                {
                    std::cout << "Got q - quit" << std::endl;
                    g_active = false;
                    break;
                }
                case SDLK_h: 
                {
                    help();
                    break;
                }
                case SDLK_n: 
                {
                    std::cout << "Got n - sending ping" << std::endl;
                    g_client->ping();
                    break;
                }
                case SDLK_a:
                {
                    std::cout << "Got a - fetching HEAD of alarmlist" << std::endl;
                    //Use sequence number from response to implement pagination
                    std::string alarmimage_seq_nr = "";
                    g_client->alarmlist(alarmimage_seq_nr.c_str(),10);
                    break;
                }
                case SDLK_u:
                {
                    static bool state = true;
                    std::cout << "Got u - (un)subscribe" << std::endl;
                    std::cout << (state?"Subscribing":"Unsubscribing")<< " from alarms" << std::endl;
                    g_client->subscribe(MxPEG_SubscriptionType::st_alarmupdate,state);
                    state = ! state;
                    break;
                }
                case SDLK_s:
                {
                   std::cout << "current session statistics: " << std::endl;
                   MX_SessionStatistics stat = g_client->get_session_statistics();
                   std::cout << "- network input queue sizes (packets as received from the camera) -" << std::endl;
                   std::cout << "   video:   " << stat.network_queue_size_video << std::endl;
                   std::cout << "   audio:   " << stat.network_queue_size_audio << std::endl;
                   std::cout << "   control: " << stat.network_queue_size_control << std::endl;
                   std::cout << "- overflow prevention -" << std::endl;
                   std::cout << "   dropped packets from network queues: " << stat.network_packets_dropped << std::endl;
                   std::cout << "   dropped frames before decoding:      " << stat.decoder_frames_dropped << std::endl;
                    break;
                }
                case SDLK_KP_MINUS:
                {
                   std::cout << "deactivating audio" << std::endl;
                   g_client->setAudioActive(false);
                    break;
                }
                case SDLK_KP_PLUS:
                {
                   std::cout << "activating audio" << std::endl;
                   g_client->setAudioActive(true);
                    break;
                }
                case SDLK_KP_1:
                {
                   std::cout << "requesting pcm16 audio" << std::endl;
                   g_client->setAudioMode("pcm16");
                    break;
                }
                case SDLK_KP_2:
                {
                   std::cout << "requesting alaw audio" << std::endl;
                   g_client->setAudioMode("alaw");
                    break;
                }
                case SDLK_l: 
                {
                    std::cout << "Got l - switching to live stream" << std::endl;
                    g_client->startLive();
                    break;
                }
                case SDLK_p: 
                {
                    std::cout << "Got p - switching to player" << std::endl;
                    g_client->play();
                    break;
                }
                case SDLK_o:
                {
                    std::cout << "Got o - seeking - entre ts" << std::endl;
                    struct timeval ts;
                    ts.tv_sec = 1597746399;
                    ts.tv_usec = 0;
                    g_client->seek(MxPEG_SeekMode::sm_nearest,false,ts);
                    break;
                }
                case SDLK_w:
                {
                    std::cout << "Got w - switching to player using show command" << std::endl;
                    g_client->show();
                    break;
                }
                case SDLK_SPACE: 
                {
                    std::cout << "Got space - pausing playback " << std::endl;
                    g_client->pause();
                    break;
                }
                case SDLK_1: 
                {
                    rate = 1.0;
                    std::cout << "Got 1 - playback rate to " << rate << std::endl;
                    g_client->setPlaybackRate(rate);
                    break;
                }
                case SDLK_PLUS: 
                {
                    rate += 1.0;
                    std::cout << "Got + - playback rate to " << rate << std::endl;
                    g_client->setPlaybackRate(rate);
                    break;
                }
                case SDLK_MINUS: 
                {
                    rate -= 1.0;
                    std::cout << "Got - - playback rate to " << rate << std::endl;
                    g_client->setPlaybackRate(rate);
                    break;
                }
                case SDLK_RIGHT: {
                    std::cout << "Got right - Jump to next event" << std::endl;
                    /*
                     * pause the clip before jumping, without pause playback will immediately continue
                     * after the jump
                     */
                    g_client->pause();
                    g_client->playerPositionNext();
                    break;
                }
                case SDLK_LEFT: {
                    std::cout << "Got left - Jump to previous event" << std::endl;
                    /*
                     * pause the clip before jumping, without pause playback will immediately continue
                     * after the jump
                     */
                    g_client->pause();
                    g_client->playerPositionPrev();
                    break;
                }
                case SDLK_UP: {
                    std::cout << "Got up - Jump to HEAD event" << std::endl;
                    /*
                     * pause the clip before jumping, without pause playback will immediately continue
                     * after the jump
                     */
                    g_client->pause();
                    g_client->playerPositionHead();
                    break;
                }
                case SDLK_DOWN: {
                    std::cout << "Got up - Jump to TAIL event" << std::endl;
                    /*
                     * pause the clip before jumping, without pause playback will immediately continue
                     * after the jump
                     */
                    g_client->pause();
                    g_client->playerPositionTail();
                    break;
                }
            }
            break;
        }
        case SDL_MOUSEBUTTONUP:
        {
            SDL_MouseButtonEvent * mbe = (SDL_MouseButtonEvent *)&event;
            switch(mbe->button)
            {
                /*
                 * Setting the image center. 
                 * 
                 * In this example the configMode=true. Changing the center will reconfigure the camer 
                 * and affect all other sessions.
                 */
                case SDL_BUTTON_LEFT: 
                {
                    g_client->ptzSetCenter((uint32_t)mbe->x,(uint32_t)mbe->y, true);
                    //keep last click coordinates for zoom operation
                    last_x=(uint32_t)mbe->x;
                    last_y=(uint32_t)mbe->y;
                    break;
                }
                default:
                {
                }
            }
            break;
        }
        case SDL_MOUSEWHEEL:
        {
            /*
             * Zoom on active stream use last_x/last_y to select the sub-window for the zoom operation.
             */
        	//relative zoom always operates on the zoom level of the current picture, with mouse wheel operation
        	//this leads to a sluggish zoom behavior because the next zoom step is only becoming active once the
        	//previous zoom command is "visible" in the picture
            SDL_MouseWheelEvent * mwe = (SDL_MouseWheelEvent *)&event;
            if(mwe->y > 0)
            {
                std::cout << "wheel + -> zoom in" << std::endl;
                g_client->ptzSetZoom(last_x, last_y, 250, true, true);
            } else if(mwe->y < 0) {
                std::cout << "wheel - -> zoom out" << std::endl;
                g_client->ptzSetZoom(last_x, last_y, -250, true, true);
            }
            break;
        }
        default:
        {
            //std::cout << "unhandled SDL event " << event.type << std::endl;
        }
    }
    }
}


#define URL_SZ 256

int main(int argc, char** argv) {
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
    * Initialize SDL module to support video output 
    */   
   SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
   
   std::cout << "EventStream client SDK simple player example" << std::endl;

   /*
    * Print help message listing supported hot-keys
    */
   help();

   /*
    * Create a video sink object to process the decoded video frames (pass argv[1] to allow sink to set this as window title)
    */
   std::shared_ptr<SinkVideo> sinkVideo = std::shared_ptr<SinkVideo>(new SinkVideo(argv[1]));

   /*
    * Create an audio sink object to process the received audio packages
    */
   std::shared_ptr<SinkAudio> sinkAudio = std::shared_ptr<SinkAudio>(new SinkAudio());

   /*
    * Create an authorization handler that provides the credentials (if needed)
    */
   std::shared_ptr<AuthorizationHandler> authHandler = std::shared_ptr<AuthorizationHandler>(new AuthorizationHandler());

   /*
    * Create a notification listener to handle the json replys of the camera
    */
   std::shared_ptr<NotificationListener> notificationListener = std::shared_ptr<NotificationListener> (new NotificationListener());

   /*
    * Create a network connection helper
    */
   std::shared_ptr<NetworkConnection> networkConnectionHelper = std::shared_ptr<NetworkConnection> (new NetworkConnection());

   /*
    * Create a new eventstream client object
    */
   g_client = MxPEG_SDK_Factory::create(sinkAudio, sinkVideo, MxPEG_ImageMode::im_RGB, authHandler,notificationListener, networkConnectionHelper);

   /*
    * configure the client to connect to camera with hostname "i25"
    */
   g_client->stream_setup(url);

   /*
    * Disable audio support for this sample
    */
   g_client->setAudioActive(false);

   /*
    * ativate video for this session
    */
   g_client->setVideoActive(true);

   /*
    * start playback of the most recent clip
    */
   g_client->startLive();


   while(g_active){
      bool haveMoreData = false;
      {
#ifdef ASYNC_NETWORK
         std::lock_guard<std::mutex> lock(g_async_mutex);
#endif
         haveMoreData = g_client->loop();
         poll_sdl_events();
      }

      if(!haveMoreData) {
#ifdef _MSC_VER
         Sleep(20);
#else
         usleep(20000);
#endif
      }
   }
    
   SDL_Quit();

#ifdef _MSC_VER
   WSACleanup();
#endif
   return 0;
}
