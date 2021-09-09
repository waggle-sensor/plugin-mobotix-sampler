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
    

#ifndef M1IMAGE_JSIMAGE_SRC_MXPEG_AUTHORIZATIONHANDLER_HPP_
#define M1IMAGE_JSIMAGE_SRC_MXPEG_AUTHORIZATIONHANDLER_HPP_

#include <cstddef>
#include <memory>

namespace ie { namespace MxPEG
{

class MxPEG_AuthorizationHandler
{

public:
   typedef std::shared_ptr<MxPEG_AuthorizationHandler> shared_ptr_t;

   virtual ~MxPEG_AuthorizationHandler() {};

   /*
    * Copy user name to buf. Return true on success, false otherwise.
    */
   virtual bool getUserName(char * buf, size_t bufSz) = 0;

   /*
    * Copy password to buf. Return true on success, false otherwise.
    */
   virtual bool getPassword(char * buf, size_t bufSz) = 0;

   /*
    * Returns true if authentication should always be used, default is to only use authentication
    * if the webserver returns a 401 Unauthorized message.
    *
    * Please note: Even if a connection is allowed without credentials (public access), some features will
    * require authorization. In such a case the camera will return a "permission denied" error as response
    * to the json command (-> notification handler). If such an error is received the client needs to reconnect
    * while haveCredentials() returning true to enforce authentication.
    */
   virtual bool haveCredentials() = 0;

   /*
    * Is called if the authentication failed
    */
   virtual void authenticationFailed() = 0;

   /*
    * Is called if the authentication failed and connection attempt is aborted.
    */
   virtual void connectionAborted() = 0;

   /*
    * Specifies how often the network stack should try to reconnect to the camera on authentication failures
    * until it aborts the connection attempt.
    * Before each reconnect authenticationFailed() is called to allow the handler to update its credentials
    * After the last failed reconnect attempt (@see retryOnFail) connectionAborted() is called.
    *
    * Set to 0 to disable automatic retires
    */
   virtual unsigned int maxRetries() = 0;

   /*
    * Automatically reconnect when authorization is required/failed.
    *
    * Before each reconnect authenticationFailed() is called to allow the handler to update its credentials.
    */
   virtual bool reconnectOnAuthFail() = 0;
};

}}

#endif /* M1IMAGE_JSIMAGE_SRC_MXPEG_AUTHORIZATIONHANDLER_HPP_ */
