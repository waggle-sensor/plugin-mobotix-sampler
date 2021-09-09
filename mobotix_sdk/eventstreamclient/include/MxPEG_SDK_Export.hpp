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
    
#ifdef __cplusplus
extern "C" {
#endif
// Short declarations (interface) ----------------------------------------------


//! ex-/import
#define IE_EXPORT_API _IE_EXPORT_API
#define IE_IMPORT_API _IE_IMPORT_API

//! export of interface classes
#if defined (_MSC_VER)
#  define IE_INTERFACE_API IE_EXPORT_API
#else
#  define IE_INTERFACE_API
#endif

// Implementation --------------------------------------------------------------

#if defined (_MSC_VER)
#  define _IE_EXPORT_API __declspec (dllexport)
#  define _IE_IMPORT_API __declspec (dllimport)
#else // #ifdef IE_OS_WIN
#  define _IE_EXPORT_API __attribute__ ((visibility("default")))
#  define _IE_IMPORT_API __attribute__ ((visibility("default")))
#endif // #ifdef IE_OS_WIN


#ifdef __cplusplus
} // extern "C" {
#endif
