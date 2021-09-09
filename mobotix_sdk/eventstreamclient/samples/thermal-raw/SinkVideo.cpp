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
#include "MxPEG_ThermalRawData.hpp"
#include "MxPEG_Image.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#if defined (_MSC_VER)
#include <fcntl.h>
#include <io.h>
#endif

SinkVideo::SinkVideo (std::string outName)
:m_name(outName),m_count(0)
{
   std::cout << "Creating SinkVideo using " << outName << " as basename for frames and thermal raw data" << std::endl;
}

SinkVideo::~SinkVideo ()
{
   std::cout << "Deleting SinkVideo" << std::endl;
}

void SinkVideo::writeThermalData(MxPEG_Image &buffer) {

      std::shared_ptr<MX_ThermalRawData> rawData = buffer.fetchThermalRawData(MXT_Sensor::left);

      // request information about the shutter of the thermal sensor (8 bit FFC counter)
      //uint8_t ffcCount = buffer.thermalFFCCount(MXT_Sensor::right);

      if(rawData.get() != nullptr) {
         std::cout << "  Got thermal raw data from left sensor " << std::endl;
         writeThermalRaw(rawData);
         writeThermalRawIntCSV(rawData);
         writeThermalCelsiusCSV(rawData);
      }

      rawData = buffer.fetchThermalRawData(MXT_Sensor::right);
      if(rawData.get() != nullptr) {
         std::cout << "  Got thermal raw data from right sensor " << std::endl;
         writeThermalRaw(rawData);
         writeThermalRawIntCSV(rawData);
         writeThermalCelsiusCSV(rawData);
      }
}

bool SinkVideo::writeRBG(MxPEG_Image & buffer) {

   char fname[1024];
   snprintf(fname,1024,"%s_%06u_%ux%u.rgb",m_name.c_str(),m_count,buffer.width(),buffer.height());
   FILE * fVideoOut = NULL;
   fVideoOut = fopen(fname, "w");

#if defined (_MSC_VER)
   //for windows set the out stream to binary mode
   int res = _setmode(_fileno(fVideoOut), _O_BINARY);
#endif

   if ( fVideoOut != NULL )
   {
      //only write RGB data
      if ( buffer.mode() == MxPEG_ImageMode::im_RGB )
      {
         /*
          * channel order is bgra, interleaved 4:4:4, 8 bit per channel
          */
         const uint8_t * rgbBuffer = buffer.imgBuffer();
         const size_t rgbBufferSize = buffer.imgBufferSize();
         fwrite(rgbBuffer, rgbBufferSize, 1, fVideoOut);
         std::cout << "  -> wrote raw rgb image: " << rgbBufferSize << " bytes, res: " << (int)buffer.width() << "x"
               << (int)buffer.height() << std::endl;
      }
      fclose(fVideoOut);
      return true;
   }

   return false;
}

bool SinkVideo::writeThermalRaw(std::shared_ptr<MX_ThermalRawData> rawData) {

   char fname[1024];
   snprintf(fname,1024,"%s_%06u_%s_%ux%u_%s.thermal.raw",
         m_name.c_str(),
         m_count,
         ((rawData->sensor()==MXT_Sensor::left)?"left":"right"),
         rawData->width(),
         rawData->height(),
         ((rawData->bitDepth()==MXT_BitDepth::depth14bit)?"14bit":"unknown"));
   FILE * fVideoOut = NULL;
   fVideoOut = fopen(fname, "w");

   if(fVideoOut == nullptr) {
      //failed to open output file
      return false;
   }

#if defined (_MSC_VER)
   //for windows set the out stream to binary mode
   int res = _setmode(_fileno(fVideoOut), _O_BINARY);
#endif

   /*
    * channel order is bgra, interleaved 4:4:4, 8 bit per channel
    */
   const uint8_t * rawBuffer = rawData->rawBuffer();
   size_t rawBufferSize = rawData->rawBufferSize();

   fwrite(rawBuffer, rawBufferSize, 1, fVideoOut);
   std::cout << "  -> wrote thermal raw data of sensor " << (int)rawData->sensor() << " " << rawBufferSize << " bytes, res: " << (int)rawData->width() << "x"
         << (int)rawData->height() << std::endl;
   fclose(fVideoOut);
   return true;
}

bool SinkVideo::writeThermalRawIntCSV(std::shared_ptr<MX_ThermalRawData> rawData)
{
   char fname[1024];
   snprintf(fname,1024,"%s_%06u_%s_%ux%u_%s.thermal.uint.csv",
         m_name.c_str(),
         m_count,
         ((rawData->sensor()==MXT_Sensor::left)?"left":"right"),
         rawData->width(),
         rawData->height(),
         ((rawData->bitDepth()==MXT_BitDepth::depth14bit)?"14bit":"unknown"));
   FILE * fVideoOut = NULL;
   fVideoOut = fopen(fname, "w");

   if(fVideoOut == nullptr) {
      //failed to open output file
      return false;
   }

#if defined (_MSC_VER)
   //for windows set the out stream to binary mode
   int res = _setmode(_fileno(fVideoOut), _O_BINARY);
#endif

   std::ostringstream header;
   header << "sensor;" << ((rawData->sensor()==MXT_Sensor::left)?"left":"right") << "\n";
   header << "bit depth;" << ((rawData->bitDepth()==MXT_BitDepth::depth14bit)?"14 bit":"unknown") << "\n";
   header << "width;" << (int)rawData->width() << "\n";
   header << "height;" << (int)rawData->height() << "\n";
   header << "resolution;" << ((rawData->resolution()==MXT_Resolution::high)?"high":"low") << "\n";
   header << "advanced radiometry support;" << ((rawData->advancedRadiometrySupport()==true)?"yes":"no") << "\n";
   header << "unit;raw uint16" << "\n";
   header << "\n";

   fwrite(header.str().c_str(), header.str().length(), 1, fVideoOut);

   /*
    * raw layout of 14bit thermal raw:
    * format uses 2 bytes per pixel (little endian), with the 2 most significant bytes set to 0:
    * llll llll hhhh hhhh
    * xxxx xxxx 00xx xxxx
    */
   const uint8_t * rawBuffer = rawData->rawBuffer();
   size_t rawBufferSize = rawData->rawBufferSize();
   for(uint32_t y = 0; y < rawData->height(); ++y) {
      std::ostringstream line;
      for(uint32_t x = 0; x < rawData->width(); ++x) {
         uint32_t offset = y*(rawData->width()*2) + (x * 2);
         //sanity check - should never happen
         if((offset + 1) >= rawBufferSize) break;
         //make sure to clear 2 msb
         uint8_t highByte = (rawBuffer[offset+1] & 0x3F);
         uint8_t lowByte = rawBuffer[offset];
         uint32_t value = (highByte << 8) + lowByte;
         line << value;
         if(x+1 < rawData->width()) line << ";";
      }
      line << "\n";
      fwrite(line.str().c_str(), line.str().length(), 1, fVideoOut);
   }
   std::cout << "  -> converted thermal raw data of sensor " << (int)rawData->sensor() << " to integer csv file " << std::endl;

   fclose(fVideoOut);
   return true;
}

bool SinkVideo::writeThermalCelsiusCSV(std::shared_ptr<MX_ThermalRawData> rawData) {

   if(!rawData->advancedRadiometrySupport()) {
      std::cout << "conversion to absolute temperatures is only supported for sensors with advanced radiometry support" << std::endl;
      return false;
   }

   char fname[1024];
   snprintf(fname,1024,"%s_%06u_%s_%ux%u_%s.thermal.celsius.csv",
         m_name.c_str(),
         m_count,
         ((rawData->sensor()==MXT_Sensor::left)?"left":"right"),
         rawData->width(),
         rawData->height(),
         ((rawData->bitDepth()==MXT_BitDepth::depth14bit)?"14bit":"unknown"));
   FILE * fVideoOut = NULL;
   fVideoOut = fopen(fname, "w");

   if(fVideoOut == nullptr) {
      //failed to open output file
      return false;
   }

#if defined (_MSC_VER)
   //for windows set the out stream to binary mode
   int res = _setmode(_fileno(fVideoOut), _O_BINARY);
#endif

   std::ostringstream header;
   header << "sensor;" << ((rawData->sensor()==MXT_Sensor::left)?"left":"right") << "\n";
   header << "bit depth;" << ((rawData->bitDepth()==MXT_BitDepth::depth14bit)?"14 bit":"unknown") << "\n";
   header << "width;" << (int)rawData->width() << "\n";
   header << "height;" << (int)rawData->height() << "\n";
   header << "resolution;" << ((rawData->resolution()==MXT_Resolution::high)?"high":"low") << "\n";
   header << "advanced radiometry support;" << ((rawData->advancedRadiometrySupport()==true)?"yes":"no") << "\n";
   header << "unit;degrees Celsius" << "\n";

   header << "\n";

   fwrite(header.str().c_str(), header.str().length(), 1, fVideoOut);

   for(uint32_t y = 0; y < rawData->height(); ++y) {
      std::ostringstream line;
      for(uint32_t x = 0; x < rawData->width(); ++x) {
         line << rawData->convertTo(x,y,MXT_Unit::celsius);
         if(x+1 < rawData->width()) line << ";";
      }
      line << "\n";
      fwrite(line.str().c_str(), line.str().length(), 1, fVideoOut);
   }
   std::cout << "  -> converted thermal raw data of sensor " << (int)rawData->sensor() << " to Celsius csv file " << std::endl;

   fclose(fVideoOut);
   return true;

}

MxPEG_ReturnCode SinkVideo::doConsumeVideo(MxPEG_Image::unique_ptr_t buffer) {

   m_count++;

   /*
    * get the timestamp for each frame - in this example only used for debug output
    */
   uint64_t syncTime = buffer->creationTime().tv_sec;
   syncTime *= 1000000;
   syncTime += buffer->creationTime().tv_usec;

   std::cout << "received video frame #" << m_count
         << " type: " << ((buffer->mode()==MxPEG_ImageMode::im_YUV)?"YUV":"BGRA")
         << " ts: " << syncTime << std::endl;

   //write the RGB image data
   writeRBG(*buffer);

   //write the thermal data
   writeThermalData(*buffer);
   return er_Success;
}
