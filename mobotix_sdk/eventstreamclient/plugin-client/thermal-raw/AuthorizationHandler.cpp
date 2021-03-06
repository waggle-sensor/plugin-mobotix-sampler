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

#include "AuthorizationHandler.h"
#include <iostream>
#include <string>
#include <string.h>

AuthorizationHandler::AuthorizationHandler(std::string username, std::string password)
    : m_username(username), m_password(password)
{
   std::cout << "Creating AuthorizationHandler" << std::endl;
}

AuthorizationHandler::~AuthorizationHandler()
{
   std::cout << "Deleting AuthorizationHandler" << std::endl;
}

bool AuthorizationHandler::getUserName(char *buf, size_t bufSz)
{
   snprintf(buf, bufSz, "%s", m_username.c_str());
   return true;
}

bool AuthorizationHandler::getPassword(char *buf, size_t bufSz)
{
   snprintf(buf, bufSz, "%s", m_password.c_str());
   return true;
}

bool AuthorizationHandler::haveCredentials()
{
   return true;
}

void AuthorizationHandler::authenticationFailed()
{
   std::cout << "Authentication failed" << std::endl;
}

void AuthorizationHandler::connectionAborted()
{
   std::cout << "Connection aborted" << std::endl;
}

unsigned int AuthorizationHandler::maxRetries()
{
   return 3;
}

bool AuthorizationHandler::reconnectOnAuthFail()
{
   return false;
}
