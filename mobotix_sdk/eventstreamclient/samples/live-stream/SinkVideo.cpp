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

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

#if defined (_MSC_VER)
#include <fcntl.h>
#include <io.h>
#endif

SinkVideo::SinkVideo (const char * outFile)
{
   std::cout << "Creating SinkVideo" << std::endl;
   fVideoOut = NULL;
   if(outFile != NULL) {
      fVideoOut = fopen(outFile,"w");
      assert(fVideoOut != NULL);
#if defined (_MSC_VER)
      //for windows set the out stream to binary mode
      int res = _setmode(_fileno(fVideoOut), _O_BINARY);
	  std::cout << "changed output mode to binary for " << outFile << ": " << res << std::endl;
#endif
      /*
       * You can play this file using vlc
       *
       * vlc --demux rawvideo --rawvid-fps 12 --rawvid-width $WIDTH --rawvid-height $HEIGHT --rawvid-chroma I420 $FILE
       *
       * $WIDTH = width of the raw picture stream
       * $HEIGHT = height of the raw picture stream
       * $FILE = file name out the output
       */
      std::cout << "created file output for YUV video stream: " << outFile << std::endl;
   } else {
      std::cout << "sample video sink, dropping all payload data" << std::endl;
   }
}

SinkVideo::~SinkVideo ()
{
   std::cout << "Deleting SinkVideo" << std::endl;

   if(fVideoOut != NULL) {
      fclose(fVideoOut);
   }
   std::cout << "closed YUV video out file" << std::endl;
}

MxPEG_ReturnCode SinkVideo::doConsumeVideo(MxPEG_Image::unique_ptr_t buffer) {
   struct timeval syncTime = buffer->creationTime();       //timestamp for AV sync

   std::cout << "received video frame res: " << (int)buffer->width() << "x"
         << (int)buffer->height()
         << " type: " << ((buffer->mode()==MxPEG_ImageMode::im_YUV)?"YUV":"BGRA")
         << " ts: " << syncTime.tv_sec << ":" << syncTime.tv_usec <<
         " size: " << (int)buffer->imgBufferSize() << " bytes " << std::endl;

   if(fVideoOut != NULL) {
       if( buffer->mode() == MxPEG_ImageMode::im_YUV ) {
            //yuv 420 planar
            const uint8_t * yBuffer = buffer->imgBuffer();
            uint32_t  yBufferSize = buffer->width() * buffer->height();

            const uint8_t * uBuffer = yBuffer + yBufferSize;
            uint32_t  uBufferSize = buffer->width()/2 * buffer->height()/2;

            const uint8_t * vBuffer = uBuffer + uBufferSize;
            uint32_t  vBufferSize = buffer->width()/2 * buffer->height()/2;

            fwrite(yBuffer,yBufferSize,1,fVideoOut);
            fwrite(uBuffer,uBufferSize,1,fVideoOut);
            fwrite(vBuffer,vBufferSize,1,fVideoOut);
       } else if( buffer->mode() == MxPEG_ImageMode::im_RGB ) {
           //channel order is bgra, interleaved
          const uint8_t * rgbBuffer = buffer->imgBuffer();
           size_t  rgbBufferSize = buffer->imgBufferSize();
		   fwrite(rgbBuffer,rgbBufferSize,1,fVideoOut);
	   }
   }

   return er_Success;
}
