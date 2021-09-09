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

#include "MxPEG_Decoder_API.hpp"

#include <iostream>
#include <cstdio>
#include <sstream>

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined (_MSC_VER)
#include <fcntl.h>
#include <io.h>
#else
#include <unistd.h>
#endif



using namespace ie::MxPEG;

/***************************************************************************************
 *                                                                                     *
 * Example functions taken from the VideoSink class in the "thermal-raw" sample app.   *
 *                                                                                     *
 *                                                                                     *
 ***************************************************************************************/

bool writeRBG(MxPEG_Image & buffer, std::string & name) {

   char fname[1024];
   snprintf(fname,1024,"%s_%ux%u.rgb",name.c_str(),buffer.width(),buffer.height());
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
         std::cout << "  -> wrote raw rgb image: " << rgbBufferSize << " bytes, " << " res: " << (int)buffer.width() << "x"
               << (int)buffer.height() << std::endl;
      }
      fclose(fVideoOut);
      return true;
   }

   return false;
}

bool writeThermalRaw(std::shared_ptr<MX_ThermalRawData> rawData, std::string & name) {

   char fname[1024];
   snprintf(fname,1024,"%s_%s_%ux%u_%s.thermal.raw",
         name.c_str(),
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
   std::cout << "  -> wrote thermal raw data of sensor " << (int)rawData->sensor() << " " << rawBufferSize << " bytes, " << " res: " << (int)rawData->width() << "x"
         << (int)rawData->height() << std::endl;
   fclose(fVideoOut);
   return true;
}

bool writeThermalRawIntCSV(std::shared_ptr<MX_ThermalRawData> rawData, std::string & name)
{
   char fname[1024];
   snprintf(fname,1024,"%s_%s_%ux%u_%s.thermal.uint.csv",
         name.c_str(),
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

bool writeThermalCelciusCSV(std::shared_ptr<MX_ThermalRawData> rawData, std::string & name) {

   if(!rawData->advancedRadiometrySupport()) {
      std::cout << "conversion to absolute temperatures is only supported for sensors with advanced radiometry support" << std::endl;
      return false;
   }

   char fname[1024];
   snprintf(fname,1024,"%s_%s_%ux%u_%s.thermal.clecius.csv",
         name.c_str(),
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
   std::cout << "  -> converted thermal raw data of sensor " << (int)rawData->sensor() << " to celcius csv file " << std::endl;

   fclose(fVideoOut);
   return true;

}

void writeThermalData(MxPEG_Image &buffer, std::string & name) {

      std::shared_ptr<MX_ThermalRawData> rawData = buffer.fetchThermalRawData(MXT_Sensor::left);
      if(rawData.get() != nullptr) {
         std::cout << "  Got thermal raw data from left sensor " << std::endl;
         writeThermalRaw(rawData, name);
         writeThermalRawIntCSV(rawData, name);
         writeThermalCelciusCSV(rawData, name);
      }

      rawData = buffer.fetchThermalRawData(MXT_Sensor::right);
      if(rawData.get() != nullptr) {
         std::cout << "  Got thermal raw data from right sensor " << std::endl;
         writeThermalRaw(rawData, name);
         writeThermalRawIntCSV(rawData, name);
         writeThermalCelciusCSV(rawData, name);
      }
}
/***************************************************************************************
 *                                                                                     *
 * Example functions taken from the VideoSink class in the "thermal-raw" sample app.   *
 *                                                                                     *
 *                                                                                     *
 ***************************************************************************************/

int main(int argc, char** argv) {
   std::cout << "EventStream client SDK MxPEG decoder example" << std::endl;

   if(argc < 2) {
	   std::cout << "Missing parameter input file" << std::endl;
	   std::cout << "Start with " << argv[0] << " [input file] [output basename]" << std::endl;
	   std::cout << "Press enter to exit" << std::endl;
	   getchar();
	   exit(1);
   }

#ifdef _MSC_VER
   WSADATA wsaData;
   int iResult;
   iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
   if (iResult != NO_ERROR) {
	   std::cout << "WSAStartup failed: " << iResult << std::endl;
	   return 1;
   }
#endif

   char * inName = argv[1];
   std::string outFile("decoded");
   if(argc > 2) {
      outFile = argv[2];
   }

   struct stat statBuf;
   if(stat(inName,&statBuf) != 0) {
      std::cout << "Error accessing " << inName << " for reading." <<std::endl;
      return -1;
   }

   uint8_t * buffer = new uint8_t[statBuf.st_size];
   if(buffer == nullptr) {
      std::cout << "Failed to allocate read buffer of size " << statBuf.st_size << std::endl;
      return -1;
   }

   FILE * inFile = fopen(inName,"rb");
   if(!inFile) {
      std::cout << "Error opening " << inName << " for reading." <<std::endl;
      delete buffer;
      return -1;
   }

   size_t nBytesRead = fread(buffer, sizeof (uint8_t), statBuf.st_size, inFile);
   if(nBytesRead != (size_t)statBuf.st_size) {
      std::cout << "Problem loading all data, decoding might fail. Expected " << statBuf.st_size << " bytes, loaded " << nBytesRead <<std::endl;
   }
   fclose(inFile);

   /*
    * create a decoder and set output mode to RGB
    */
   MxPEG_Decoder_API::shared_ptr_t decoder = MxPEG_Decoder_Factory::create(MxPEG_ImageMode::im_RGB);

   /*
    * Setup data structure for decoder
    */
   MxPEG_Frame frame;
   frame.data = buffer;
   frame.size = nBytesRead;

   /*
    * Decode the frame, the decoded image will be similar to what is passed to the VideoSink classes in the streaming use case
    */
   MxPEG_Image::unique_ptr_t decodedImage = decoder->decodeFrame(frame);

   /*
    * we no longer need the input buffer
    */
   delete buffer;

   /*
    * Write the decoded raw data to output files
    */
   writeRBG(*decodedImage,outFile);
   writeThermalData(*decodedImage,outFile);


#ifdef _MSC_VER
   WSACleanup();
#endif
}
