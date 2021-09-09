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
    

#ifndef M1IMAGE_JSIMAGE_SRC_MXPEG_AUDIO_H_
#define M1IMAGE_JSIMAGE_SRC_MXPEG_AUDIO_H_

#include "MxPEG_Defines.hpp"
#include "MxPEG_SDK_Export.hpp"

#include <memory>
#include <time.h>


namespace ie { namespace MxPEG
{

typedef enum MxPEG_AudioPacketType_T {
   eapt_Start           = 0,    //MXA   - start of audio stream, contains stream format information
   eapt_Data            = 1,    //MXS   - playload data
   eapt_Stop            = 2,    //MXO   - end of stream
   eapt_NoData          = 3     //no valid audio data
} MxPEG_AudioPacketType;

typedef enum MxPEG_AudioCodec_T {
   eac_pcm              = 0,    //PCM16 audio data
   eac_alaw             = 1,    //alaw audio data
   eac_unsupported      = 2
} MxPEG_AudioCodec;

typedef enum MxPEG_SampleFormat_t {
  easf_U8       = 0,
  easf_S8       = 1,
  easf_S16      = 2,
  easf_U16      = 3,
  easf_S32      = 4,
  easf_F32      = 5,
  easf_unsupported =6
} MxPEG_SampleFormat;


/****************************************************************
 *
 * MxPEG_AudioFormat
 *
 ****************************************************************/
class IE_INTERFACE_API MxPEG_AudioFormat
{

public:
      MxPEG_AudioFormat (
            MxPEG_AudioCodec codec = eac_pcm,
            MxPEG_SampleFormat sampleFormat = easf_S16,
            uint32_t sampleFreq = 16000,
            uint32_t channels = 1);
   MxPEG_AudioFormat(MxPEG_AudioFormat * format);
   MxPEG_AudioFormat(const MxPEG_AudioFormat & format);

   virtual MxPEG_AudioCodec codec();
   virtual uint32_t samplefreq();
   virtual MxPEG_SampleFormat sampleFormat();
   virtual uint32_t sampleDuration();
   virtual uint32_t channels();
   virtual uint8_t bytesPerSample();

   virtual bool operator==( const MxPEG_AudioFormat& other);

   virtual ~MxPEG_AudioFormat();

protected:
   MxPEG_AudioCodec audioCodec;
   uint32_t audioSamplefreq;
   MxPEG_SampleFormat audioSampleFormat;
   uint32_t audioChannels;
};

/****************************************************************
 *
 * MxPEG_Audio
 *
 * Interface for audio container class
 *
 ****************************************************************/
// cppcheck-suppress noCopyConstructor
class IE_INTERFACE_API MxPEG_Audio
{
public:

   typedef std::unique_ptr<MxPEG_Audio> unique_ptr_t;

   /*
    * Returns pointer to audio payload data stored in this container
    */
   virtual
   uint8_t * audioBuffer() = 0;

   /*
    * Size of audio payload data stored in this container
    */
   virtual
   size_t audioBufferSize()= 0;

   /*
    * Access the raw pointer to the buffer holding the audio data
    */
   virtual
   uint8_t * rawBuffer() = 0;

   /*
    * Size of the internal buffer used for the audio data.
    */
   virtual
   size_t rawBufferSize() = 0;

   /*
    * clone the object including all internal audio buffers
    */
   virtual
   MxPEG_Audio * clone() = 0;
   
   /*
    * the timestamp when the first byte of audio data of this buffer was recorded
    */
   virtual struct timeval timeStamp() = 0;

   virtual MxPEG_AudioFormat *format() = 0;
   virtual uint32_t duration_ms()= 0;

   virtual ~MxPEG_Audio () {};
};

}} /* namespace ie::MxPEG */

#endif /* M1IMAGE_JSIMAGE_SRC_MXPEG_AUDIO_H_ */
