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

#include "SinkVideo.h"
#include "MxPEG_Defines.hpp"    
#define MX_LOG_MODULE MX_LOG_VIDEO

#include "MxPEG_Image.hpp"

#include <iostream>
#include <sstream>
#include "string.h"

#include <SDL.h>

SinkVideo::SinkVideo (std::string name)
   :cameraName(name), renderedFrames(0), surface_width(320), surface_height(240)
{
   sdlScreen = nullptr;
   sdlRenderer = nullptr;
   sdlTexture = nullptr;

}

SinkVideo::~SinkVideo ()
{

   SDL_DestroyTexture(sdlTexture);
   SDL_DestroyRenderer(sdlRenderer);
   SDL_DestroyWindow(sdlScreen);
    
}

MxPEG_ReturnCode SinkVideo::doConsumeVideo(MxPEG_Image::unique_ptr_t bitmap) {

   renderedFrames++;

   //std::cout <<"canvas sink consuming bitmap " << renderedFrames << ": " << bitmap->width() << "x" << bitmap->height() << " bytes: " << bitmap->imgBufferSize() << std::endl;

   if(sdlScreen == nullptr || sdlRenderer == nullptr || sdlTexture == nullptr ||
         surface_width != bitmap->width() || surface_height != bitmap->height())
   {
      surface_width = bitmap->width();
      surface_height = bitmap->height();

      std::cout <<"clearing SDL structures" << std::endl;
      SDL_DestroyTexture(sdlTexture);
      SDL_DestroyRenderer(sdlRenderer);
      SDL_DestroyWindow(sdlScreen);

      std::cout <<"initializing SDL" << std::endl;
      SDL_CreateWindowAndRenderer(surface_width,surface_height,SDL_WINDOW_RESIZABLE,&sdlScreen,&sdlRenderer);

      if( ( sdlScreen == nullptr ) && ( sdlRenderer == nullptr ) ) {
         throw "failed to create SDL environment";
      }

      sdlTexture = SDL_CreateTexture(sdlRenderer,
                                     SDL_PIXELFORMAT_ARGB8888,
                                     SDL_TEXTUREACCESS_STREAMING,
                                     surface_width, surface_height);

      if( sdlTexture == nullptr ) {
         throw "Failed to create SDL texture";
      }
      std::cout <<"SDL successfully initialized" << std::endl;
   }

   if( ( sdlScreen != nullptr ) && ( sdlRenderer != nullptr ) ) {
      char frameTime[64];

      /*
       * get recordTime (= time image was recorded by camera) and show this as title in the output window
       */
      time_t recTime = bitmap->recordTime().tv_sec;
      struct tm * recTime_tm;
      recTime_tm = localtime(&recTime);
      strftime(frameTime,64,"%F %T",recTime_tm);

      std::ostringstream ossTitle("Eventstream player: ");
      ossTitle << cameraName << " " << frameTime;


      //std::cout <<"drawing to SDL window" << std::endl;
      SDL_UpdateTexture(sdlTexture, NULL, bitmap->imgBuffer(), surface_width * sizeof (uint32_t));
      SDL_RenderClear(sdlRenderer); //is this needed??
      SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
      SDL_RenderPresent(sdlRenderer);
      SDL_SetWindowTitle(sdlScreen,ossTitle.str().c_str());
   } else {
      std::cout << "ERROR: no SDL surface" << std::endl;
   }

   return er_Success;
}

