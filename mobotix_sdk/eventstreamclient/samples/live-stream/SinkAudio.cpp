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

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

#if defined (_MSC_VER)
#include <fcntl.h>
#include <io.h>
#endif


SinkAudio::SinkAudio (const char * outFile)
{
   std::cout << "Creating SinkAudio" << std::endl;
   //right now only PCM16 at 16kHz, mono is supported
   audio_format = new MxPEG_AudioFormat(eac_pcm, easf_U16, 16000, 1);

   fAudioOut = NULL;

   if(outFile != NULL) {
      fAudioOut = fopen(outFile,"w");
      assert(fAudioOut != NULL);
#if defined (_MSC_VER)
      //for windows set the out stream to binary mode
      int res = _setmode(_fileno(fAudioOut), _O_BINARY);
	  std::cout << "changed output mode to binary for " << outFile << ": " << res << std::endl;

#endif
      /*
       * You can play the audio output file with
       *
       * vlc --demux=rawaud --rawaud-channels 1 --rawaud-samplerate 16000 $FILE
       *
       * $FILE = file name out the output
       */
      std::cout << "created file output for raw PCM16 audio stream: " << outFile << std::endl;
   } else {
      std::cout << "sample audio sink, dropping all payload data" << std::endl;
   }
}

SinkAudio::~SinkAudio ()
{
   std::cout << "Deleting SinkAudio" << std::endl;
   if(fAudioOut != NULL) {
      fclose(fAudioOut);
   }
   std::cout << "Closed raw audio out file" << std::endl;
}

MxPEG_ReturnCode SinkAudio::closeAudioDevice() {
   return er_Success;
}

MxPEG_ReturnCode SinkAudio::initAudioDevice() {
   return er_Success;
}

MxPEG_ReturnCode SinkAudio::doConsumeAudio(MxPEG_Audio::unique_ptr_t buffer) {
   struct timeval syncTime = buffer->timeStamp();

   std::cout << "received audio data "
         << " ts: " << syncTime.tv_sec << ":" << syncTime.tv_usec
         << (int)buffer->audioBufferSize() << " bytes " << std::endl;

   if(fAudioOut != NULL) {
      uint8_t * audioData = buffer->audioBuffer();
      size_t  audioDataSize = buffer->audioBufferSize();
      fwrite(audioData,audioDataSize,1,fAudioOut);
   }

   return er_Success;
}

MxPEG_AudioFormat * SinkAudio::doGetTargetFormat() {
   return audio_format;
}

