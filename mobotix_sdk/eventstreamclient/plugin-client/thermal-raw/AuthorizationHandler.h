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

#ifndef SAMPLE_AUTHORIZATIONHANDLER_H_
#define SAMPLE_AUTHORIZATIONHANDLER_H_

#include <string>

#if defined(_MSC_VER)
#include <Windows.h>
#endif
#include <MxPEG_AuthorizationHandler.hpp>

using namespace ie::MxPEG;
/*
 * Sample authorization handler, provides hard coded user name and password
 */
class AuthorizationHandler : public MxPEG_AuthorizationHandler
{
public:
   AuthorizationHandler(std::string username, std::string password);

   virtual ~AuthorizationHandler();

   virtual bool getUserName(char *buf, size_t bufSz);

   virtual bool getPassword(char *buf, size_t bufSz);

   virtual bool haveCredentials();

   virtual void authenticationFailed();

   virtual void connectionAborted();

   virtual unsigned int maxRetries();

   virtual bool reconnectOnAuthFail();

private:
   std::string m_username;
   std::string m_password;
};

#endif /* SAMPLE_AUTHORIZATIONHANDLER_H_ */
