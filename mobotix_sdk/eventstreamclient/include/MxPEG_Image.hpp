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
     //  See LICENSE_SDK included in the SDK package
     //
     //  Copyright (c) 2005 - 2016, MOBOTIX AG
     //  All rights reserved.
     //
///////
    


#ifndef MXPEG_SRC_MXPEG_IMAGE_HPP_
#define MXPEG_SRC_MXPEG_IMAGE_HPP_

#include "MxPEG_Defines.hpp"
#include "MxPEG_ThermalRawData.hpp"
#include "MxPEG_SDK_Export.hpp"

#include <memory>
#include <vector>
#include <time.h>
#include <map>

enum class IE_INTERFACE_API MxPEG_ImageMode {
   im_RGB = 0,				//RGB, using channel oder bgra interleaved
   im_YUV,				//YUV420 picture data
   im_YUVmx				//MX internal YUV format, uses 4 byte / pixel (like RGBa), stores RGBa.r=Y,RGBa.g=Cr,RGBa.b=Cb => shader needs load only one texture
};

namespace ie { namespace MxPEG
{

   // cppcheck-suppress noCopyConstructor
   class IE_INTERFACE_API MxPEG_Image {

   public:

      typedef std::unique_ptr<MxPEG_Image> unique_ptr_t;

      virtual ~MxPEG_Image() {};

      /*
       * returns the width of the image
       */
      virtual uint32_t width() = 0;

      /*
       * returns the height of the image
       */
      virtual uint32_t height() = 0;

      /*
       * get access to the raw pointer of the RGB/YUV image
       */
      virtual void getImgBuffer(uint8_t** buffer, size_t *bufferSize) = 0;

      /*
       * get access to the raw pointer of the RGB/YUV image
       */
      virtual size_t imgBufferSize() = 0;

      /*
       * get the size of the raw buffer that stores the RGB/YUV image
       */
      virtual const uint8_t*imgBuffer() = 0;

      /*
       * returns a shared_ptr to the thermal raw data of the specified sensor. If no data is available for this sensor, the returned
       * shared_ptr is initialized with nullptr.
       */
      virtual std::shared_ptr<MX_ThermalRawData> fetchThermalRawData(MXT_Sensor sensor) = 0;

      /*
       * returns the 8 bit counter of the shutter of the thermal sensor (FFC)
       */
      virtual uint8_t thermalFFCCount(MXT_Sensor sensor) = 0;

      /*
       * returns the time the RGB/YUV image was created
       */
      virtual struct timeval creationTime() = 0;

      /*
       * returns the time the RGB/YUV image was recorded
       */
      virtual struct timeval recordTime() = 0;

      /*
       * returns the decoding mode that was used for this image. Possible options are YUV and RGB
       */
      virtual MxPEG_ImageMode mode() = 0;

   };

}}

#endif /* MXPEG_SRC_MXPEG_IMAGE_HPP_ */
