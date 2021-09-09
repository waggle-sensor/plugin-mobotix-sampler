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

#ifndef SAMPLE_SINKVIDEO_H_
#define SAMPLE_SINKVIDEO_H_

#ifdef WINDOWS
#include <Windows.h>
#endif

#include "MxPEG_SinkVideo.hpp"
#include "MxPEG_Image.hpp"
#include "MxPEG_Defines.hpp"

#include <string>

using namespace ie::MxPEG;
/*
 * Sample video sink, writes the decoded frames and thermal raw data using to the specified base name.
 */
class SinkVideo : public MxPEG_SinkVideo
{
public:
   /*
    * Constructor of the sample sink, takes the base name for the files generated as parameter.
    */
   SinkVideo (std::string outName);

   virtual
   ~SinkVideo ();

protected:
   /*
    * Receives each video frame right after it was decoded. Either YUV or RGB is possible (see MxPEG_Image.hpp and MxPEG_SDK_API.hpp)
    */
   virtual
   MxPEG_ReturnCode doConsumeVideo(MxPEG_Image::unique_ptr_t buffer);

private:
   /*
    * Writes the image data of decoded to RGB (see main.cpp) to an output file. A new file is created for each frame.
    */
   bool writeRBG(MxPEG_Image &buffer);

   /*
    * Writes the thermal raw data that is included with each frame to an output file. Separate files are generated for each frame.
    * If multiple thermal sensors are connected to the camera, the output is written to separate files for each sensor.
    */
   void writeThermalData(MxPEG_Image &buffer);

   /*
    * Writes the thermal raw data as a binary blob to an output file.
    */
   bool writeThermalRaw(std::shared_ptr<MX_ThermalRawData> rawData);
   /*
    * Converts the thermal raw data to unsigned integer values for each pixel. Writes integer values to a csv file.
    */
   bool writeThermalRawIntCSV(std::shared_ptr<MX_ThermalRawData> rawData);
   /*
    * If supported (TR sensor in "Linear Mode"):
    * Converts the thermal raw data to degrees Celsius float values for each pixel. Writes integer values to a csv file.
    */
   bool writeThermalCelsiusCSV(std::shared_ptr<MX_ThermalRawData> rawData);

   std::string m_name;
   uint32_t m_count;
};

#endif /* SAMPLE_SINKVIDEO_H_ */
