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
/*
 * NetworkConnection.cpp
 *
 *  Created on: Feb 17, 2017
 *      Author: gauss
 */

#include "NetworkConnection.h"

#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <iostream>

#ifndef _MSC_VER
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netdb.h>
#endif


#ifdef ASYNC_NETWORK
extern std::mutex g_async_mutex;

void async_recv(int socket, NetworkConnection * connection) {
   while(connection->recv_active) {
      struct timeval tv;
      tv.tv_sec = 1;
      tv.tv_usec = 0;
      fd_set sockset;
      FD_ZERO(&sockset);
      FD_SET(socket, &sockset);
      int result = select(socket + 1, &sockset, NULL, NULL, &tv);
      if(result < 0) {
         std::lock_guard<std::mutex> lock(g_async_mutex);
         connection->sendNetworkError(socket,errno,"select failed on socket");
      } else if (result > 0) {
         std::lock_guard<std::mutex> lock(g_async_mutex);
         connection->sendDataAvailable(socket);
      } else {
         std::cout << "No data available on socket - waiting" << std::endl;
      }
   }
}
#endif


NetworkConnection::NetworkConnection()
{
#ifdef ASYNC_NETWORK
   recv_active = false;
#endif
   std::cout << "creating new connection helper" << std::endl;
}

NetworkConnection::~NetworkConnection(){
#ifdef ASYNC_NETWORK
   std::cout << "stopping async recv thread" << std::endl;
   recv_active = false;
   recv_thread.join();
#endif
   std::cout << "removing connection helper" << std::endl;
};


/*
 * as part of the main loop, the SDK checks if new data is available from the
 * network interface (using select). Apart from that the SDK will also read data
 * from the network interface each time the sendDataAvailable() callback is triggered.
 *
 * To disable the automatic check, return true here.
 *
 * To activate the asynchronous receive example set -DASYNC_NETWORK=1 in the Makefile (or VisualStudio)
 */
bool NetworkConnection::messageDrivenReceive() {
#ifdef ASYNC_NETWORK
   return true;
#else
   return false;
#endif
}

NetworkResult NetworkConnection::recvData (int handle, void *buffer, size_t bufferSize, size_t *bytesReceived) {
   if(handle <=0 ||buffer == nullptr) return result_Failed;

   size_t rc = recv(handle, (char *)buffer, bufferSize, 0);
   if(bytesReceived) *bytesReceived = rc;
   if(rc == 0) return result_NoData;
   return ( rc > 0 )?result_Success:result_Failed;
}

NetworkResult NetworkConnection::sendData (int fd, const void *buffer, size_t bufferSize, int *bytesSent) {
   if(fd <=0 ||buffer == nullptr) return result_Failed;

   size_t rc = send(fd, (const char *)buffer, bufferSize, 0);
   if(bytesSent) *bytesSent = rc;
   return ( rc == bufferSize )?result_Success:result_Failed;
}

NetworkResult NetworkConnection::setupConnectionToRemoteHost(const char * hostName, int port, bool ssl, int * sock) {
   if(hostName == nullptr || port < 0 || port > 65535 || sock == nullptr) return result_Failed;

   if(ssl) {
      std::cout << "SSL support not implemented!!!!" << std::endl;
      return result_Failed;
   }

   int nRc;
    *sock = (int)socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    std::cout << "socket is " << *sock << std::endl;
    if(*sock <= 0) {
       std::cout << "Error creating socket" << std::endl;
       nRc = -1;
       return result_Failed;
    }

    std::cout << "connecting to " << hostName << " using socket " << *sock << std::endl;
   struct hostent *host;
   host = gethostbyname(hostName);
   if(host == nullptr) {
      std::cout << "Error resolving host name " << hostName << std::endl;
      return result_Failed;
   }

   struct sockaddr_in *sockAddr;
   sockAddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
   if(sockAddr == nullptr) {
      std::cout << "Error: out of memory?"<< std::endl;
#ifdef _MSC_VER
      closesocket(*sock);
#else
      close(*sock);
#endif
      *sock = 0;
      return result_Failed;
   }
   memset(sockAddr, 0, sizeof(struct sockaddr_in));

   sockAddr->sin_port=htons(port);
   sockAddr->sin_family=AF_INET;
   sockAddr->sin_addr.s_addr = *((unsigned long*)host->h_addr);

   std::cout <<  "Connecting... ";
   nRc = connect(*sock,(struct sockaddr *)sockAddr,sizeof(struct sockaddr));
   std::cout << "connect returned: " << nRc<< std::endl;
   if(nRc >= 0) {
      std::cout <<  "Connection ready" << std::endl;
   } else {
#ifdef _MSC_VER
      nRc = WSAGetLastError();
      std::cout << "WSA connect error: " << nRc << std::endl;
#endif
      if( errno == EINPROGRESS ) {
         std::cout << "Connection in progress" << std::endl;
         //wait ??
      } else {
         std::cout << "Connection failed: " << errno << std::endl;
         sendNetworkError(*sock,errno,strerror(errno));
      }
   }

   free(sockAddr);

   /*
    * tell the SDK that the connection is ready for use
    */
   std::cout << "triggering connection ready callback"<< std::endl;
   sendConnectionReady(*sock);

#ifdef ASYNC_NETWORK
   recv_active = true;
   recv_thread = std::thread(async_recv, *sock, this);
#endif

   return result_Success;
}

NetworkResult NetworkConnection::shutdownConnectionToRemoteHost(int handle) {

   if(handle > 0) {
#ifdef _MSC_VER
      closesocket(handle);
#else
      close(handle);
#endif
     sendConnectionClosed(handle);
     return result_Success;
   } else {
      std::cout << "called close without connection" << std::endl;
   }
   return result_Failed;
}

void NetworkConnection::notifyNetworkStatus(ConnectionStatus status) {
   switch(status) {
      case status_Closing:
         std::cout << "got notification: closing connection" << std::endl; break;
      case status_Closed:
         std::cout << "got notification: connection closed" << std::endl; break;
      case status_Connecting:
         std::cout << "got notification: opening connection" << std::endl; break;
      case status_Connected:
         std::cout << "got notification: connection ready" << std::endl; break;
      case status_Error:
         std::cout << "got notification: connection error" << std::endl; break;
   }
}
