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
    

#ifndef MXPEG_SRC_MXPEG_SINK_HPP_
#define MXPEG_SRC_MXPEG_SINK_HPP_

#include "MxPEG_Image.hpp"
#include <memory>

namespace ie { namespace MxPEG
{

/*
 * Interface of the Video Sink.
 *
 * Pass an instance of this to the constructor when creating an instance of MxPEG_SDK(). The SDK will pass all decoded video frames to the doConsumeVideo()
 * function of that instance.
 *
 */
class MxPEG_SinkVideo
{

public:
   typedef std::shared_ptr<MxPEG_SinkVideo> shared_ptr_t;

   virtual ~MxPEG_SinkVideo() {};

   /*
    * Receives each video frame right after it was decoded. Either YUV or RGB is possible (see MxPEG_Image.hpp and MxPEG_SDK_API.hpp)
    *
    */
   virtual
   MxPEG_ReturnCode doConsumeVideo(MxPEG_Image::unique_ptr_t buffer) = 0 ;

};

} } /* namespace ie::MxPEG */

#endif /* MXPEG_SRC_MXPEG_SINK_HPP_ */
