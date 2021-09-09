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
 * MxPEG_ConnectionHandler.hpp
 *
 *  Created on: Feb 16, 2017
 *      Author: gauss
 */

#ifndef SRC_MXPEG_NETWORKCONNECTION_HPP_
#define SRC_MXPEG_NETWORKCONNECTION_HPP_

#include "MxPEG_Defines.hpp"

#include <cstddef>
#include <memory>

typedef void (*cb_net_status)(int socket, void * userData);
typedef void (*cb_net_error)(int socket, int errNo, const char* msg, void * userData);


namespace ie { namespace MxPEG
{

enum NetworkResult {
  result_Success,
  result_NoData,
  result_Failed
};

enum ConnectionStatus {
   status_Closing,
   status_Closed,
   status_Connecting,
   status_Connected,
   status_Error
};

class NetworkCallbacks {

public:
   NetworkCallbacks()
      :cb_connection_ready(nullptr),
      cb_dataAvailable(nullptr),
      cb_closed(nullptr),
      cb_error(nullptr),
      userData(nullptr)
      {}

   public:
      cb_net_status     cb_connection_ready;
      cb_net_status     cb_dataAvailable;
      cb_net_status     cb_closed;
      cb_net_error      cb_error;
      void              *userData;
   };


   class MxPEG_NetworkConnection {

   public:
      typedef std::shared_ptr<MxPEG_NetworkConnection> shared_ptr_t;

      MxPEG_NetworkConnection();

      virtual
      ~MxPEG_NetworkConnection();

      /*
       * Return true to disable internal check for new data on socket (polling for data on each loop). Data will
       * only be fetched if cb_dataAvailable is triggered.
       */
      virtual
      bool messageDrivenReceive() = 0;

      /*
       * Create a network connection to the specified host / port. The ssl flag is set true if the stream url that is passed 
       * to the steam_setup() API function starts with "https://". 
       * 
       * Return: A handle that will be passed along the MxPEG_NetworkConnection in each call. 
       */
      virtual
      NetworkResult setupConnectionToRemoteHost(const char * hostName, int port, bool ssl, int * handle) = 0 ;

      /*
       * Fetch data and place it in the buffer and return the number of received bytes in the "bytesReceived" parameter.
       * 
       * If no data is available, this function should return result_NoData. Do not block in this function. If polling is userData
       * a little pause (<=10ms) to wait for data may be acceptable 
       */
      virtual
      NetworkResult recvData (int handle, void *buffer, size_t bufferSize, size_t *bytesReceived) = 0;

      /*
       * Send the data passed in the buffer. Return the number of bytes that actually have been sent in bytesSent.
       */
      virtual
      NetworkResult sendData (int handle, const void *buffer, size_t bufferSize, int *bytesSent) = 0 ;

      /*
       * Close the network connection.
       */
      virtual
      NetworkResult shutdownConnectionToRemoteHost(int handle) = 0 ;

      /*
       * The notification mechanism is not fully implemneted - ignore this for now.
       */
      virtual
      void notifyNetworkStatus(ConnectionStatus status);

      /*
       * The SDK will use this to register its callback functions for status changes
       */
      void setCallbacks(NetworkCallbacks & callbacks);

      /*
       * use these functions to trigger callbacks in the SDK
       */
      bool sendConnectionReady(int handle);
      bool sendDataAvailable(int handle);
      bool sendConnectionClosed(int handle);
      bool sendNetworkError(int handle, int errNo, const char* msg);

   private:
      NetworkCallbacks m_callbacks;

   };
}}




#endif /* SRC_MXPEG_NETWORKCONNECTION_HPP_ */
