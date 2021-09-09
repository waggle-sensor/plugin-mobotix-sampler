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
    

#ifndef M1IMAGE_JSIMAGE_SRC_MXPEG_SDK_API_HPP_
#define M1IMAGE_JSIMAGE_SRC_MXPEG_SDK_API_HPP_


#include "MxPEG_SinkVideo.hpp"
#include "MxPEG_SinkAudio.hpp"
#include "MxPEG_AuthorizationHandler.hpp"
#include "MxPEG_EventNotificationListener.hpp"
#include "MxPEG_NetworkConnection.hpp"
#include "MxPEG_SDK_Export.hpp"
/*
 * Statistics of the current streaming session.
 */
typedef struct {
   int32_t network_queue_size_video;      //number of unprocessed data packets received from the camera with image content
   int32_t network_queue_size_audio;      //number of unprocessed data packets received from the camera with audio content
   int32_t network_queue_size_control;    //number of unprocessed data packets received from the camera with control data
   int32_t network_packets_dropped;       //number of data packets dropped from the network queues to avoid memory overruns
   int32_t decoder_frames_dropped;        //number of frames dropped before decoding
} MX_SessionStatistics;

#include <memory>
#include <string>

namespace ie { namespace MxPEG {

class IE_INTERFACE_API MxPEG_SDK
{
public:
   typedef std::shared_ptr<MxPEG_SDK> shared_ptr_t;

   virtual ~MxPEG_SDK () {};

   /*
    * Main loop of the SDK package, returns after one "iteration". Needs to be called repeatedly from
    * the applications main loop.
    *
    * If a message driven connection helper is used: Returns tue if there is more data pending to be processed in the input queues.
    *
    * If network polling (standard) is used: returns always true.
    */
   virtual bool loop() = 0;

   /*
    * Setup the library internally.
    *
    * @url URL of the EventStream service. Typically this should look like this:
    *
    * http://<cam-ip>:<port>/control/eventstream.jpg
    *
    * Return er_Success on success
    */
   virtual MxPEG_ReturnCode stream_setup(const char *url) = 0;


   /*
    * Establish a connection with the camera. (Implicitly called by stream_setup)
    *
    * Returns er_Success if the connection could be established
    */
   virtual MxPEG_ReturnCode stream_connect() = 0;

   /*
    * Close the connection.
    *
    * @reconnect: Immediately try to connect to the service after closing the connection.
    *
    * Returns er_Success, or er_* error code.
    */
   virtual MxPEG_ReturnCode stream_close(bool reconnect = false) = 0;


   /*
    * Get a new id to be used for when calling queue_command()
    *
    * Return: On success positive integer number to be used as id for queue_command(), on error -1
    */
   virtual int get_command_id() = 0;

   /*
    * Send a low-level message/command to the camera. Using this function any command available from
    * the EventStream service can be invoked.
    *
    * @id The id used to tag the command, the camera will use this id when sending replies. Use get_command_id to get a valid, unique id number
    * @ctrlCmd string holding a JSON message to be sent to the camera.
    *
    * Returns er_Success, or er_* error code.
    *
    * Please note: The return type "split" for generic commands is not yet supported in this library.
    */
   virtual MxPEG_ReturnCode queue_command(int id, const char * ctrlCmd) = 0;

   /*
    * Enable debug logging in the SDK. Log messages will be printed to stdout. See MxPEG_Logger.hpp for supported values for
    * target and mask.
    *
    * Mask ids of the modules in the SDK:
    *
    *	SYSTEM           1
	*	NET              2
	*	VIDEO            4
	*	AUDIO            8
	*	C_API            16
	*	DECODER          32
	*	EVENT            64
    *
    * @mask Sum of all log mask ids.
    *
    * Returns er_Success, or er_* error code.
    */
   virtual MxPEG_ReturnCode set_log_mask(int mask) = 0;

   /*
    * Audio return channel. Play back audio on the internal speaker of the camera (only models with integrated speaker).
    *
    * Start a new audio return session using the specifed codec. Suported are eac_pcm for pcm16 with 16000 Hz sample
    * rate and eac_alaw for aLaw with 8000 Hz
    *
    * Returns er_Success, or er_* error code.
    */
   virtual MxPEG_ReturnCode sendAudioStart(MxPEG_AudioCodec sendCodec = eac_pcm) = 0;

   /*
    * Stop the audio session.
    *
    * Returns er_Success, or er_* error code.
    */
   virtual MxPEG_ReturnCode sendAudioStop() = 0;

   /*
    * Send audio raw data to the camera.
    *
    * Depending on the "sendCodec" that is specified in the sendAudioStart() call and the format passed to
    * this function internal conversion routines will be used to recode the audio data.
    *
    * @format: the format of the audio data passed to this function (right now only PCM16LE, 16kHz, 1 channel and alaw, 8kHz, 1 channel are supported)
    * @rawAudio: Buffer holding raw audio data as specified by format
    * @rawAudioSize: Size of the audio buffer
    *
    * Returns er_Success, or er_* error code.
    */
   virtual MxPEG_ReturnCode sendAudio(MxPEG_AudioFormat format, uint8_t *rawAudio, size_t rawAudioSize) = 0;
   virtual MxPEG_ReturnCode sendAudio(MxPEG_AudioFormat format, uint8_t *rawAudio, size_t rawAudioSize, struct timeval ts) = 0;
   /*
    * Activate video for the current session. Without this the camera will not send any video frames.
    *
    * @active "true" activates video transmission for the current session, "false" disables it.
    *
    * Returns On success: the command id associated with the request, 0 otherwise
    */
   virtual uint32_t setVideoActive(bool active) = 0;

   /*
    * Activate thermal raw data export for the current session.
    *
    * Note 1: To use this feature the camera must be equipped with at least one thermal sensor that is configured
    * for raw export. (Open the camera setup menu and navigate to: Setup->Thermal Sensor Settings, set the option "Thermal Raw Data" to
    * enabled or to recording.
    *
    * Note 2: Thermal raw data is transmitted together with video frames. To receive thermal raw data, also call
    * setVideoActive(true)
    *
    * @active "true" activates video transmission for the current session, "false" disables it.
    *
    * Returns On success: the command id associated with the request, 0 otherwise
    */
   virtual uint32_t setThermalRawExportActive(bool active) = 0;


   /*
    * Set the encoding mode for the video frames.
    *
    * @mode: Video encoding mode for this session. Supported modes are MJPEG, MxPEG (default)
    *
    * Return: On success the command id associated with the request, 0 otherwise
    */
   virtual uint32_t setVideoMode(MxPEG_VideoMode mode) = 0;

   /*
    * Set the video resolution for the stream.
    *
    * @width Width of the video stream
    * @height Height of the video stream
    * @configmode: true to allow global reconfiguration of the camera
    *
    * If configmode is "false" the size change is limited to the current session, max resolution is the
    * currently configured "Image Size".
    *
    * If configmode is "true" this will change the image size globally.
    *
    * Returns On success: the command id associated with the request, 0 otherwise
    *
    */
   virtual uint32_t setResolution(uint32_t width, uint32_t height, bool configMode = false) = 0;


   /*
    * Set fps for the stream.
    *
    * @fps Frame rate at which to generate the video stream
    *
    * Returns On success: the command id associated with the request, 0 otherwise
    *
    */
   virtual uint32_t setVideoFps(float fps) = 0;


   /*
    * Set the sensor for the video stream.
    *
    * @sensor: Select the active sensor. Valid options depend on the camera model and equipped sensors.
    *
    * Note: This option automatically implies configmode = true!
    *
    * Returns On success: the command id associated with the request, 0 otherwise
    *
    */
   virtual uint32_t setVideoSensor(MxPEG_Sensor sensor) = 0;


   /*
    * Set the display_mode for the video stream.
    *

    * @displayMode: string specifying the display mode for the video stream. Valid options are:
    *
    * 	"normal": Shows the full image of the selected image sensor. For hemispheric lenses, the original distorted full image is displayed.
    * 	"lenscorr": Shows a corrected image from non-hemispheric lenses in 4:3 format.
    *	"surround": Shows four images with independent views.
    *	"panorama": Shows a panorama image in 8:3 format.
    *	"pano_dbl": Shows two panorama images in 8:3 format.
    *	"pano_focus": Shows one panorama image in 8:3 format and two 4:3 images below.
    *	"pip": Shows a small preview image of the other sensor.
    *	"piz": Shows a small overview image in the zoomed image.
    *	"eip": Shows the last event image in the live image.
    *
    * @configmode: true to allow global reconfiguration of the camera
    *
    * Calling this function with configmode "false" will only have an effect if displaymode is globally
    * set to "Full Image".
    *
    * Note: Depending on your camera model, some of these features may not be available!
    *
    * Returns On success: the command id associated with the request, 0 otherwise
    *
    */
   virtual uint32_t setVideoDisplayMode(std::string displayMode, bool configMode = true) = 0;
   //use this variant for windows:
   virtual uint32_t setVideoDisplayMode(const char * displayMode, bool configMode = true) = 0;

   /*
    * Enable/Disable audio for the current session.
    *
    * @active "true" to activate audio, "false" otherwise
    *
    * Returns On success: the command id associated with the request, 0 otherwise
    *
    */
   virtual uint32_t setAudioActive(bool active) = 0;

   /*
    * Configures the audio output mode for the current session. Supported modes are
    *
    * @mode String that specifies the type of audio data used in this session
    *
    * "pcm16": Send audio data as PCM16 little endian
    * "alaw": Audio data using aLaw encoding
    *
    * Please note: This affects only Audio encoding for the live video stream! Recordings will
    * always include aLaw encoded audio!
    *
    * Please Note: To actually receive audio you need to call setAudioActive(true)
    *
    * Returns On success: the command id associated with the request, 0 otherwise
    *
    */
   virtual uint32_t setAudioMode(std::string mode) = 0;
   //use this variant for windows:
   virtual uint32_t setAudioMode(const char * mode) = 0;

   /*
    * Start streaming the live feed from the cameras currently active sensor
    *
    * Returns On success: the command id associated with the request, 0 otherwise
    *
    */
   virtual uint32_t startLive() = 0;;

   /*
    * Subscribe/Unsubscribe to events on the camera 
    *
    * MxPEG_SubscriptionType event_type: One of:
    *	st_elight: Door Station only: Change of state of external light
    * st_door: Door Station only: Change of state of door bell
    * st_alarmupdate: Alarm events, same format as returned by the AlarmList command
    * st_config: Notifies config changes (json path and format as used by kurator)
    * 
    * bool active: Use to sub- / unsubscribe
    * true: subscribe
    * false: unsubscribe
    */
   virtual uint32_t subscribe(MxPEG_SubscriptionType event_type, bool active) = 0;

   /*
    * Request a list of recent alarms from the camera:
    * 
    * const char * start_sequence: Sequence number of first alarmimage to consider in the search. Leave empty
    * to start the search from HEAD.
    *
    * uint32_t num_alarms: Max. number of alarms to include in response
    * 
    * The response will contain a list of alarm events like this:
    *
    * {
    *       "alarmimage": "002474.0",
    *       "date": "2012-10-22",
    *       "events": "VM",
    *       "eventlist": [
    *           {
    *               "name": "VM",
    *               "eventid": "VM",
    *               "type": "IMA"
    *           }
    *       ],
    *       "time": "14:02:42",
    *       "timestamp": "1350907362.164"
    *   }
    */
   virtual uint32_t alarmlist(const char * start_sequence, uint32_t num_alarms) = 0;

   /*
    * Seek to the specified timestamp. 
    * 
    * MxPEG_SeekMode mode: Specified the algorithen to select the target position:
    *       sm_exact: Seek to frame with a ts that the exactly matches the the specified or return an error (#27 "clip not found")
    *       sm_nearest: Seek to frame that is closest to the specified timestamp
    *       sm_previous: Seek to frame that is closest to the specified timestamp - consider only frames with ts <= the specified timestamp
    *       sm_next: Seek to frame that is closest to timestamp - consider only frames with ts >= the specified timestamp
    * bool fast: Fast search - position on the start of the clip containing the timestamp (instead of the exact frame) 
    * 
    * struct timeval ts: Timestamp to seek for.
    */
   virtual uint32_t seek(MxPEG_SeekMode mode, bool fast, struct timeval ts) = 0;

   /*
    * DEPRECATED! play(time) will be removed in the future
    *
    * Use seek() to position the player on the requested position, then 
    * use play() to start the stream. 
    *
    *
    * Start playback of the recording at startTime
    *
    * @startTime: Timestamp from which to start playback. If no recording is available from
    * the exact time, the next recoding after that point in time is started.
    *
    * Returns On success: the command id associated with the request, 0 otherwise
    */
   virtual uint32_t play(struct timeval startTime) = 0;



   /*
    * Start playback of the recording at the current player position
    *
    * Returns On success: the command id associated with the request, 0 otherwise
    *
    */
   virtual uint32_t play() = 0;

   /*
    * Pauses the current playback session.
    *
    * Returns On success: the command id associated with the request, 0 otherwise
    *
    */
   virtual uint32_t pause() = 0;

   /*
    * Show starts the stream from the current time position and pauses the stream after the first frame. Use this to skip through a stream frame by frame.
    * To skip backwards through the stream, set the playback rate to -1
    *
    * Returns On success: the command id associated with the request, 0 otherwise
    *
    */
   virtual uint32_t show() = 0;


   /*
    * Ping camera to test network connection.
    *
    * The camera will reply with a pong message the refers to the message id returned by the ping.
    *
    * The notification listener will get a message like this:
    *  {"result":["pong"],"error":null,"id":<ID>}
    */
   virtual uint32_t ping() = 0;

   /*
    * Sets the playback rate of the current playback session.
    *
    * @rate Specifies the playback rate. Use values greater 1 to implement fast
    * forward and values between 0 and 1 to implement "slow motion". Use negative values to implement
    * (fast) reverse playback. Setting the rate to 0 causes the stream to pause.
    *
    * Note: Audio is only available if the playback rate is set to 1.
    *
    */
   virtual uint32_t setPlaybackRate(float rate) = 0;

   /*
    * Position the player on the HEAD (most recent) event.
    */
   virtual uint32_t playerPositionHead() = 0;

   /*
    * Position the player on the TAIL (oldest available) event.
    */
   virtual uint32_t playerPositionTail() = 0;

   /*
    * Move the player position from the current position to the next (more recent) event.
    */
   virtual uint32_t playerPositionNext() = 0;

   /*
    * Move the player position from the current position to the previous (older) event.
    */
   virtual uint32_t playerPositionPrev() = 0;

   /*
    * Zoom in/out either relatively or set zoom value absolute.
    *
    * @x: x-coordinate of region of interest (origin top-left)
    * @y: y-coordinate of region of interest (origin top-left)
    * @zoom Choose zoom values from [1000...8000] to set zoom from 1x up to 8x
    * @configmode: true to allow global reconfiguration of the camera
    *
    * If the display mode is configured to show multiple windows in one stream (e.g. Surround) the
    * region of interest is used to determine the sub-window in which to
    * execute the zoom operation.
    *
    * If configMode is "false" the zoom will only use the current picture stream as input. If it is set to
    * "true" the zoom operation might also reconfigure the camera to create higher resolution input pictures.
    *
    * Please note: Zooming in sub-windows always requires configMode=true.
    *
    * Please further note: The camera does not digitally enlarge the picture that is generated by the sensor. If
    * the zoom operation reaches that limit, the camera will start cropping the video to the region the would
    * be the input of such a digital zoom operation. But it is up to the client to actually enlarge the cropped
    * video.
    *
    */
   virtual uint32_t ptzSetZoom(uint32_t x, uint32_t y, int32_t zoom, bool zoomRelative = false, bool configMode = false) = 0;

   /*
    * Set image center to image coordinates x,y (origin top left corner)
    *
    * @x: x-coordinate of image center (origin top-left)
    * @y: y-coordinate of image center (origin top-left)
    */
   virtual uint32_t ptzSetCenter(uint32_t x, uint32_t y, bool configMode  = false) = 0;

   /*
    * Provides access the to current streams M1IMG data. The requested data field is specified
    * by section and property name. If the field is not available NULL is returned.
    *
    * The caller needs to release the string.
    */
   virtual char * stream_info_m1img(char * section, char * property) = 0;

   /*
    * Returns the timestamp of the most recent event stored on the camera (extracted from the MXF headers).
    *
    * This function only returns valid data while the live video stream is active.
    */
   virtual uint32_t stream_info_last_event() = 0;

   /*
    * Returns session statistics - for debugging only.
    */
   virtual MX_SessionStatistics get_session_statistics() = 0;

};


class IE_INTERFACE_API MxPEG_SDK_Factory {
public:
   /*
    * Creates a new instance of the SDK object. The handler classes are passed as shared pointers.
    *
    * If you implement your own low level network layer use this to pass a custom network connection helper to the SDK. (For example to add ssl support)
    */
   static MxPEG_SDK::shared_ptr_t create (MxPEG_SinkAudio::shared_ptr_t audioSink, MxPEG_SinkVideo::shared_ptr_t sinkVideo, MxPEG_ImageMode decodingMode,
         MxPEG_AuthorizationHandler::shared_ptr_t authHandler, MxPEG_EventNotificationListener::shared_ptr_t notificationListener,
         MxPEG_NetworkConnection::shared_ptr_t connectionHelper);

   /*
    * Creates a new instance of the SDK object. The handler classes are passed as shared pointers.
    */
   static MxPEG_SDK::shared_ptr_t create (MxPEG_SinkAudio::shared_ptr_t audioSink, MxPEG_SinkVideo::shared_ptr_t sinkVideo, MxPEG_ImageMode decodingMode,
         MxPEG_AuthorizationHandler::shared_ptr_t authHandler, MxPEG_EventNotificationListener::shared_ptr_t notificationListener);

};

}}

#endif /* M1IMAGE_JSIMAGE_SRC_MXPEG_SDK_API_HPP_ */
