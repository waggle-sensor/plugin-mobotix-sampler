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
    

#ifndef MX_THERMAL_RAW_DATA_HPP_
#define MX_THERMAL_RAW_DATA_HPP_

#include "MxPEG_SDK_Export.hpp"
#include <vector>

namespace ie { namespace MxPEG
{

enum class IE_INTERFACE_API MXT_BitDepth {
   depth14bit,
   unknown
};

enum class IE_INTERFACE_API MXT_Sensor {
   right,
   left
};

enum class IE_INTERFACE_API MXT_Resolution {
   high,
   low
};

enum class IE_INTERFACE_API MXT_Unit {
   celsius,
   fahrenheit
};

class IE_INTERFACE_API MX_ThermalRawData {
public:

  virtual ~MX_ThermalRawData() {};

  /*
   * Returns the bit depth of the thermal sensor. Right now only one sensor type with 14 bit is supported.
   *
   * The 14 bit sensor uses 2 bytes per pixel with the two most significant bits set to zero.
   * The raw data is returned line by line from left to right.
   *
   */
  virtual MXT_BitDepth bitDepth() = 0;

  /*
   * The sensor that generated the thermal image (left/right)
   */
  virtual MXT_Sensor sensor() = 0;

  /*
   * Resolution of the advanced radiometry thermal sensor: high or low
   */
  virtual MXT_Resolution resolution() = 0;

  /*
   * Flag that indicates whether advanced radiometry is supported or not.
   *
   * Requirements for advanced radiometry:
   * - "TR" type sensor
   * - "Linear Mode" in thermal sensor settings needs to be activated (only available for TR type sensors)
   *
   * returns true if the thermal sensor supports advanced radiometry features.
   */
  virtual bool advancedRadiometrySupport() = 0;

  /*
   * Width of the thermal image
   */
  virtual uint32_t width() = 0;

  /*
   * Height of the thermal image
   */
  virtual uint32_t height() = 0;

  /*
   * Returns a reference to the raw data buffer and its size.
   *
   * The data layout of the raw 14bit thermal images:
   *
   * The format uses 2 bytes per pixel (little endian), with the 2 most significant bytes set to 0. Data is
   * stored in rows from left to right starting with top left pixel.
   *
   * y=0       : x=0               x=1                 ....   x=width()
   * bit-mask  : 00hhhhhh llllllll 00hhhhhh llllllll   ....   00hhhhhh llllllll
   * y=1       : x=0               x=1                 ....   x=width()
   * bit-mask  : 00hhhhhh llllllll 00hhhhhh llllllll   ....   00hhhhhh llllllll
   *    .
   *    .
   *    .
   * y=height(): x=0               x=1                 ....   x=width()
   * bit-mask  : 00hhhhhh llllllll 00hhhhhh llllllll   ....   00hhhhhh llllllll
   */
  virtual size_t rawBufferSize() = 0;
  virtual const uint8_t* rawBuffer() = 0;

  /*
   * Converts a single pixel of the raw data buffer to the specified unit.
   *
   * Note: Conversion is only supported for sensors with advanced radiometry feature! For sensors without advanced
   * radiometry support (enable "Linear Mode" in sensor settings) this function will return an empty buffer.
   *
   * x/y coordinates start by 0 from top left corner of the image.
   *
   * @x column number of the pixel
   * @y row number of the pixel
   * @unit Temperature unit to convert to
   *
   * @return Temperature of the specified pixel converted to the specified unit. Returns NaN if pixel
   * coordinates are invalid or if the sensor does not support advanced radiometry
   */
  virtual float convertTo(uint32_t x, uint32_t y, MXT_Unit unit) = 0;

};

}}
#endif // MX_THERMAL_RAW_DATA_HPP_
