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

#include "SinkAudio.h"

#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <time.h>

SinkAudio::SinkAudio ()
{
   /*
    * right now only 2 output formats are supported:
    *
    */

   /*
    * 1. Convert all audio to: PCM16 at 16kHz, mono
    */
    //audio_format = new MxPEG_AudioFormat(eac_pcm, easf_S16, 16000, 1);

   /*
    * 2. passthrough mode (depending on stream setup and usage audio will be either: A-Law, 8kHz, mono or PCM16, 16kHz, mono)
    */
   audio_format = nullptr;

   std::cout << "sample audio sink, dropping all payload data" << std::endl;
}

SinkAudio::~SinkAudio ()
{
   if(audio_format) delete audio_format;
}

MxPEG_ReturnCode SinkAudio::closeAudioDevice() {
   return er_Success;
}

MxPEG_ReturnCode SinkAudio::initAudioDevice() {
   return er_Success;
}

MxPEG_ReturnCode SinkAudio::doConsumeAudio(MxPEG_Audio::unique_ptr_t buffer) {

   std::cout << "received audio data: "
         << " codec: " << ((buffer->format()->codec()==eac_pcm)?"pcm16":((buffer->format()->codec()==eac_alaw)?"alaw":"unknown"))
         << " ch: " << buffer->format()->channels()
         << " freq: " << buffer->format()->samplefreq()
         << " sfmt: " << buffer->format()->sampleFormat()
         << " byt/sam: " << (int)buffer->format()->bytesPerSample()
         << " bytes: " << (int)buffer->audioBufferSize() << std::endl;

   return er_Success;
}

MxPEG_AudioFormat * SinkAudio::doGetTargetFormat() {
   return audio_format;
}

