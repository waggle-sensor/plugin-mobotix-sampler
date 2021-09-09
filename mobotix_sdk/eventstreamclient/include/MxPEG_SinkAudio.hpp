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
    

#ifndef M1IMAGE_JSIMAGE_SRC_MXPEG_SINKAUDIO_HPP_
#define M1IMAGE_JSIMAGE_SRC_MXPEG_SINKAUDIO_HPP_

#include "MxPEG_Audio.hpp"
#include <vector>
#include <memory>

namespace ie { namespace MxPEG
{

class MxPEG_SinkAudio
{
public:
   typedef std::shared_ptr<MxPEG_SinkAudio> shared_ptr_t;

   virtual ~MxPEG_SinkAudio () {};

   /*
    * Called by the SDK if an audio stream end notification is sent by the camera.
    */
   virtual
   MxPEG_ReturnCode closeAudioDevice() = 0;

   /*
    * Called if the SDK detects an incoming audio stream.
    */
   virtual
   MxPEG_ReturnCode initAudioDevice() = 0;

   /*
    * Receives each audio frame right after it was decoded.
    *
    */
   virtual
   MxPEG_ReturnCode doConsumeAudio(MxPEG_Audio::unique_ptr_t buffer) = 0;

   /*
    * Returns the audio format that is expected by the sink.
    *
    * Naturally the camera sends audio data as PCM16, 16kHz, mono stream. The SDK has some (limited) conversion
    * capabilities, use this to specify the desired output.
    *
    * Return nullptr to activate pass through mode. Audio data will be passed to the doConsumeAudio function
    * without conversion. The type of data is stored along each buffer that is passed to the sink and may change
    * mid stream.
    */
   virtual
   MxPEG_AudioFormat * doGetTargetFormat() = 0;

};

}} /* namespace ie::MxPEG */

#endif /* M1IMAGE_JSIMAGE_SRC_MXPEG_SINKAUDIO_HPP_ */
