/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __CXPSESSION_H__
#define __CXPSESSION_H__

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __XPSESSION_H__

/* Declare some defines for types used internal to XPSession */
#define kXPSessionUnknownID  (-1)
#define kXPSessionBroadcastID (-2)

/* These are the internal message types for session messages */
#define kXPSessionPacketType_Unreliable		(0)
#define kXPSessionPacketType_Reliable		(1<<0)
#define kXPSessionPacketType_Certain		(1<<1)

/* Error values */
#define kXPSession_EOK (0)
#define kXPSession_EERROR (-1)
#define kXPSession_ETRANSPORT_CLOSED (-2)
#define kXPSession_ETRANSPORT_ERROR (-3)

#define kTransportAnyGameChannel (0)

#endif

/* The type define for the handle that has to be passed in to each function so that the API knows what session you are trying to access */
typedef struct sCXPSession *hCXPSession;

/* What follows are 'C' style function wrappers for the C++ class */

/**
 * Allocates a new session to be used
 * \return the new session handle that is allocated
 */
hCXPSession CXPSession_Allocate(void);

/**
 * Frees a session that is allocated
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 */
void CXPSession_Free(const hCXPSession handle);

/**
 * Creates a new session using the default protocols
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param name The name to give to this session
 */
void CXPSession_Create(const hCXPSession handle,const char *name);

/**
 * Creates a new session using the supplied protocols
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param name The name to give to this session
 * \param protocols The list of protocols to use separated by commas. e.g. "UDP@,STREAMTCPIP@"
 */
void CXPSession_CreateWithProtocol(const hCXPSession handle,const char *name,const char *protocols);

/**
 * Exports a URL that can be used to connect to this session in future
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param nameBuffer the buffer to receive the name
 * \param length the maximum length of the name buffer
 */
void CXPSession_ExportURL(const hCXPSession handle,char *nameBuffer,const int length);

/**
 * Tries to connect this session to the supplied URL
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param url the url of the session to connect to
 */
void CXPSession_Join(const hCXPSession handle,const char *url);

/**
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * Tries to find sessions using the default protocols
 */
void CXPSession_Find(const hCXPSession handle);

/**
 * Tries to find sessions using the listed protocols
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param protocols The list of protocols to use separated by commas. e.g. "UDP@,STREAMTCPIP@"
 */
void CXPSession_FindWithProtocols(const hCXPSession handle,const char *protocols);

/**
 * Enumerates any found sessions and fills in the name in to the name buffer
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param nameBuffer the buffer to receive the name
 * \param length the maximum length of the name buffer
 * \return returns 1 if there was a name or 0 if not
 */
int CXPSession_EnumerateFound(const hCXPSession handle,char *nameBuffer,const int length);

/**
 * Gets the current channel number for this session
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return the channel number
 */
int CXPSession_GetGameChannel(const hCXPSession handle);

/**
 * Sets the current channel number for this session. If no channel number is passed then TransportAnyGameChannel is used.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param channel The channel number to use. Use kTransportAnyGameChannel to use any channel number
 */
void CXPSession_SetGameChannel(const hCXPSession handle,const int channel);

/**
 * Gets the session unqiue ID
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return the session unique ID
 */
int CXPSession_GetSessionID(const hCXPSession handle);

/**
 * This allows the user o check if the session is really stable.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return returns 1 if the session is stable
 */
int CXPSession_IsStable(const hCXPSession handle);

/**
 * Gets the session status.
 * A session error happens when the master session disconnects and a new master cannot be found.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return the session status of kXPSession_EOK or kXPSession_EERROR
 */
int CXPSession_GetStatus(const hCXPSession handle);

/**
 * Sends data using the 'normal' packet type from this session to any other session via it's ID or to all sessions by using XPSessionBroadcastID
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param sessionid the session id to send to or XPSessionBroadcastID
 * \param data the pointer to the data to send
 * \param length the length of the data to send
 */
void CXPSession_DataSendToSessionUnreliable(const hCXPSession handle,const int sessionid,const char *data,const int length);

/**
 * Sends data using the 'Reliable' packet type from this session to any other session via it's ID or to all sessions by using XPSessionBroadcastID
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param sessionid the session id to send to or XPSessionBroadcastID
 * \param data the pointer to the data to send
 * \param length the length of the data to send
 */
void CXPSession_DataSendToSessionReliable(const hCXPSession handle,const int sessionid,const char *data,const int length);

/**
 * Sends data using the 'Certain' packet type from this session to any other session via it's ID or to all sessions by using XPSessionBroadcastID
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param sessionid the session id to send to or XPSessionBroadcastID
 * \param data the pointer to the data to send
 * \param length the length of the data to send
 */
void CXPSession_DataSendToSessionCertain(const hCXPSession handle,const int sessionid,const char *data,const int length);

/**
 * Receives data and responds with what session sent this data
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param fromsessionid the session id that sent this data
 * \param data the buffer for the data
 * \param length the length of the data received
 * \param type a pointer that can receive the packet type. This with be kXPSessionPacketType_Unreliable, kXPSessionPacketType_Certain or kXPSessionPacketType_Reliable. This parameter can be null is which case the type is not filled in.
 * \return returns 1 if data was received and 0 if not
 */
int CXPSession_DataReceive(const hCXPSession handle,int *const fromsessionid,char *const data,int *const length,unsigned char *const type);

/**
 * Tests to see if this session is a master session
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return returns 1 is the session is a master session, 0 if not
 */
int CXPSession_IsMaster(const hCXPSession handle);

/**
 * Gets the session id that recently joined the network session
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return the session id or kXPSessionUnknownID to indicate no more session ids changed
 */
int CXPSession_GetJoiner(const hCXPSession handle);

/**
 * Gets the session id that left the network session
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return the session id or kXPSessionUnknownID to indicate no more session ids changed
 */
int CXPSession_GetLeaver(const hCXPSession handle);

/**
 * Gets the current session time. This time is synchronised between all users of the session
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return the float time  1.0 units == 1 second
 */
double CXPSession_GetTime(const hCXPSession handle);

/**
 * Gets the current local time. This time is local to the session and is not corrected by the master clock
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return the float time  1.0 units == 1 second
 */
double CXPSession_GetLocalTime(const hCXPSession handle);

/**
 * Gets the current network send transfer rate in bytes per second
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return bytes per second
 */
float CXPSession_GetNetworkSendRate(const hCXPSession handle);

/**
 * Gets the current network receive transfer rate in bytes per second
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return bytes per second
 */
float CXPSession_GetNetworkReceiveRate(const hCXPSession handle);

/**
 * Gets the number of packets that are lost and lost and resent
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return number of packets
 */
int CXPSession_GetNetworkPacketsLost(const hCXPSession handle);

/**
 * This polls XPSession. Normally an application will yield enough time during threads for XPSession to function.
 * Sometimes an application that uses a lot of 3D graphics can lock out other threads for quite some time.
 * In these cases you may call XPSession::Poll() to yield some time to XPSession to perform housekeeping.<br>
 * Single threaded considerations:<br>
 * When using the single threaded libraries this method should be called regularly. Unless ReplicaNet::Poll() is called in which case this Poll() method is already called.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 */
void CXPSession_Poll(const hCXPSession handle);

/**
 * Sets the manual Poll() method for this class only.<br>
 * For multi-threaded libraries this turns off the automatic polling method.<br>
 * This allows an application to time exactly when polls are done.<br>
 * The application must call Poll() on a regular basis. A good place is just before or after the frame update is done.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 */
void CXPSession_SetManualPoll(const hCXPSession handle);

/**
 * Sets the automatic Poll() method
 * In multi-threading libraries this restarts the automatic polling thread so that an application does not need the call Poll()<br>
 * In single-threading libraries this method has no effect and does not start a thread.<br>
 * The default multi-threading library behaviour is to enable multi-threading automatic polls,<br>
 * The default single-threading library behaviour is to use manual polls,<br>
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 */
void CXPSession_SetAutomaticPoll(const hCXPSession handle);

/**
 * This allows the poll method to be set for the XPURL layer that is used by this session.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 */
void CXPSession_SetManualPollXPURL(const hCXPSession handle);

/**
 * This allows the poll method to be set for the XPURL layer that is used by this session.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 */
void CXPSession_SetAutomaticPollXPURL(const hCXPSession handle);


/**
 * This allows the user to enable or disable spidering of network connections
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param canSpider Pass in 1 to enable spidering or 0 to disable spidering<br>
 * The default is 1
 */
void CXPSession_SetCanSpider(const hCXPSession handle,const int canSpider);

/**
 * This allows the spidering state to be read
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return the status of the spidering state
 */
int CXPSession_GetCanSpider(const hCXPSession handle);

/**
 * This allows the session to set if it can become a master due to a fault being found from another session
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param canBeMaster Pass in 1 to enable this session to become a master if needed.<br>
 * Currently a session can only become a master if it is enabled for spidering however.
 */
void CXPSession_SetCanBecomeMaster(const hCXPSession handle,const int canBeMaster);

/**
 * Allows the state to be read
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return the status of the becoming a master state
 */
int CXPSession_GetCanBecomeMaster(const hCXPSession handle);

/**
 * This allows the latency to the master session to be read
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return the latency to the master session
 */
float CXPSession_GetLatencyToMasterSession(const hCXPSession handle);

/**
 * This allows the latency recalculation delay to be set. The default value is 1 second meaning there is a ping packet sent every second.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param seconds The number of seconds before latency is recalculated.
 */
void CXPSession_SetLatencyRecalculationDelay(const hCXPSession handle,const float seconds);

/**
 * Enables or disables encryption for this session
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param enable set this to be 1 to enable encryption for packets sent from this session. The default is 0
 */
void CXPSession_SetEncryption(const hCXPSession handle,const int enable);

/**
 * Gets the status of the encryption flag for this session
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 */
int CXPSession_GetEncryption(const hCXPSession handle);

/**
 * This forces a disconnect from the supplied sessionID or if the sessionID is not supplied then all session nodes are disconnected. The disconnect is immediate and initiates the fault tolerant mechanism for XPSession and ReplicaNet on other connected session nodes. This means if the session was a master session node then the other session nodes will try to find a new master if this mechanism is enabled. In the case of a ReplicaNet session node having objects, these objects will attempt to use the fault recovery and migration if it is enabled.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param sessionID The sessionID to disconnect from. To disconnect from all sessionIDs use kXPSessionUnknownID.
 */
void CXPSession_Disconnect(const hCXPSession handle,const int sessionID);

/**
 * Allows the status of the pre-connect mechanism to be tested.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return When CXPSession_GetPreConnectStatus() returns 1 after this session node starts a join then this session node is in a pre-connect state.
 */
int CXPSession_GetPreConnectStatus(const hCXPSession handle);

/**
 * Enables or disables the pre-connect mechanism for session nodes. When this mechanism is enabled the joining ReplicaNet session node will be able to exchange packets with the master session node before the node properly joins the session. The function PreConnectHasFinished() can be called to signal that the node wishes to complete a connect to the session. This function should only be used before a session node is used to join the session.<br>
 * The intended for this mechanism is as follows:<br>
 * 1) CXPSession_SetPreConnect(1) and join a session.<br>
 * 2) When CXPSession_GetPreConnectStatus() returns 1 after this session node starts a join then this session node is in a pre-connect state.<br>
 * 3) This node will send one or more user defined packets that signal that the session wants to exchange certain pre-connect data to the master session using CXPSession_DataSendToSession() and session ID kXPSessionUnknownID.<br>
 * 4) The master session will receive user data packets and detecting that pre-connect data has been received will in reply send a user data message to the effect that it has received this information.<br>
 * 5) The joining node receives pre-connect data from the master session, verifies the data and either completes the pre-connect by calling PreConnectHasFinished() or uses Disconnect() to abort the connection.<br>
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param enable Set this to be 1 to enable the pre-connect mechanism for this session node. If 0 then the session node will discard all preconnect packets and the preconnection mechanism is assumed to be disabled. The default is 0.
 */
void CXPSession_SetPreConnect(const hCXPSession handle,const int enable);

/**
 * Allows the status of the pre-connect mechanism for this session node.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return Returns 1 if the pre-connect mechanism is enabled.
 */
int CXPSession_GetPreConnect(const hCXPSession handle);

/**
 * If the pre-connect mechanism is enabled then this function must be called by the joining session node when packets have been exchanged to the satisfaction of the joining session node.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 */
void CXPSession_PreConnectHasFinished(const hCXPSession handle);

/**
 * Enables or disables the automatic packet compression test. If enabled XPSession will test each packet sent to see if it can be compressed and if there is a space saving then the packet is compressed before sending.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param enable Enable or disable automatic packet compression. The default is 0 to disable automatic packet compression. To enable use 1
 */
void CXPSession_SetAutomaticPacketCompression(const hCXPSession handle,const int enable);

/**
 * Gets the value configured by SetAutomaticPacketCompression()
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return Returns 1 if the automatic packet compression is enabled.
 */
int CXPSession_GetAutomaticPacketCompression(const hCXPSession handle);

/**
 * Gets the compression statistics totals for packets sent from this session node.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param before The total number of bytes before compression. This can be NULL.
 * \param after The total number of bytes after compression. This can be NULL.
 * \param reset If 1, after returning the information to the caller both totals are reset back to zero. The default value is 0
 */
void CXPSession_GetCompressionStatistics(const hCXPSession handle,int *const before,int *const after,const int reset);

/**
 * Gets the merged packet statistics totals for connections managed by this session node.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param sent The total number of merged sent packets. This can be null.
 * \param received The total number of merged sent packets. This can be null.
 * \param reset If 1, after returning the information to the caller both totals are reset back to zero. The default value is 0
 */
void CXPSession_GetMergedStatistics(const hCXPSession handle,int *const sent,int *const received,const int reset);

/**
 * Gets the master session unique ID. If the session is not connected then the return value is kXPSessionUnknownID.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return the master session unique ID
 */
int CXPSession_GetMasterSessionID(const hCXPSession handle);

/**
 * Attempts to resolve the URL of the session unique ID from the point of view of this session node.
 * If the session unique ID is not known to the session then the result is a null string.
 * If the session unique ID is not directly connected to this node, for example the spider connection was broken, then this function is not guaranteed to produce a URL and can return a null string.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param sessionID The session ID.
 * \param url A pointer for the destination string for the URL. This can be NULL
 * \param urlLen The maximum size for the URL string. This can be NULL
 * \return The length of the string including the null terminating character. If there is not enough space as specified by urlLen then the space required is still returned.
 */
int CXPSession_GetURLFromSessionID(const hCXPSession handle,const int sessionID,char *const url,const int urlLen);

/**
 * Sets the client only flag for this session node. The default value is 0 which means this node will get joiner and leaver messages. If this is set to be 1 then this node will only get joiner/leaver messages for spider nodes or nodes that can become master nodes. This must not be changed after the session is used for joining a session.
 * Setting this to option to be 1 also automatically sets SetCanBecomeMaster() and SetCanSpider() to be 0.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param isClient The default value is 0.
 */
void CXPSession_SetClientOnly(const hCXPSession handle,const int isClient);

/**
 * Gets the client only configuration set by SetClientOnly().
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \return The configuration set by SetClientOnly().
 */
int CXPSession_GetClientOnly(const hCXPSession handle);

/**
 * Attempts to read the configured bandwidth for a route to a specific session ID. If the session ID is not directly connected to this node then this function will return 0 and the contents of the output and input pointers will be left unchanged.
 * If this node can supply bandwidth information about a session ID then this function will return 1 and the bandwidth, output, input pointers will contain the values as used by SetBandwidthLimit(), SetMaximumOutputBandwidth() and SetMaximumInputBandwidth(). Note that output and input bandwidth will appear to be exchanged because the output of one end of transport connection is really the input of the other end of the connection.
 * \param handle the handle for this session, returned from CXPSession_Allocate()
 * \param sessionID The session ID of the required node.
 * \param bandwidth A pointer to accept the transport bandwidth value, this can be NULL.
 * \param output A pointer to accept the output bandwidth value, this can be NULL.
 * \param input A pointer to accept the input bandwidth value, this can be NULL.
 * \return Success returns 1, failure returns 0.
 */
int CXPSession_GetBandwidthFromSessionID(const hCXPSession handle,const int sessionID,int *const bandwidth,int *const output,int *const input);

#ifdef __cplusplus
}
#endif

#endif
