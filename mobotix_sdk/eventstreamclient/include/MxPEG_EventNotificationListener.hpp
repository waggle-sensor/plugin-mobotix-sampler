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
    


#ifndef M1IMAGE_JSIMAGE_SRC_MXPEG_EVENTNOTIFICATIONLISTENER_HPP_
#define M1IMAGE_JSIMAGE_SRC_MXPEG_EVENTNOTIFICATIONLISTENER_HPP_

#include <memory>

namespace ie
{
namespace MxPEG
{

class MxPEG_EventNotificationListener
{
public:
   typedef std::shared_ptr<MxPEG_EventNotificationListener> shared_ptr_t;

   virtual
   ~MxPEG_EventNotificationListener () {};

   /*
    * Is called to inform listener for each event message (including command results) that is received from the camera.
    */
   virtual void notifyEvent(int id, const char * message) = 0;
};


} /* namespace MxPEG */
} /* namespace ie */

#endif /* M1IMAGE_JSIMAGE_SRC_MXPEG_EVENTNOTIFICATIONLISTENER_HPP_ */
