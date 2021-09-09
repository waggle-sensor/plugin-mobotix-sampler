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
    
/*
 * MxPEG_Decoder.hpp
 *
 *  Created on: Oct 8, 2018
 *      Author: gauss
 */

#ifndef SDK_INCLUDE_MXPEG_DECODER_API_HPP_
#define SDK_INCLUDE_MXPEG_DECODER_API_HPP_

#include <memory>

#include "MxPEG_Image.hpp"
#include "MxPEG_SDK_Export.hpp"

namespace ie { namespace MxPEG
{
   class IE_INTERFACE_API MxPEG_Decoder_API
   {
   public:
      typedef std::shared_ptr<MxPEG_Decoder_API> shared_ptr_t;

      virtual
      ~MxPEG_Decoder_API() {};

      /*
       * throws std::runtime_error if decoding fails
       */
      virtual
      MxPEG_Image::unique_ptr_t decodeFrame(const MxPEG_Frame & mxpegFrame) = 0;
   };


   class IE_INTERFACE_API MxPEG_Decoder_Factory {
   public:
      /*
       * Creates a new instance of a MxPEG decoder class.
       *
       */
      static MxPEG_Decoder_API::shared_ptr_t create (MxPEG_ImageMode decodingMode);
   };

}}

#endif /* SDK_INCLUDE_MXPEG_DECODER_API_HPP_ */
