/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __XPSESSION_H__
#define __XPSESSION_H__

#define _XPSESSION_VERSION 7200

#include <string>
#include <vector>
#include <list>
#include <map>
#include "RNXPSession/Inc/XPSession.h"
#include "RNXPURL/Inc/XPURL.h"
#include "RNXPURL/Inc/Transport.h"
#include "RNPlatform/Inc/MessageHelper.h"
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNPlatform/Inc/SysTime.h"

namespace RNReplicaNet
{

const int kPingHistorySize = 5;

class Thread;

/**
 * Generic session message header
 */
struct SessionPacket_Header
{
	short mDestinationSession;				/**< Destination session ID for this packet */
	short mSourceSession;					/**< Source session ID for this packet */
	unsigned char mPacketTypeAndChecksum;	/**< The internal type of the packet as a bit field */
											/*	The top 4 bits are a checksum produced from the data in the packet */
};

class XPSessionInternalBufferedPacket
{
public:
	XPSessionInternalBufferedPacket(const int sessionid,const char *data,const int length,const bool reliable,const bool certain,const bool ordered,const unsigned int band);
	~XPSessionInternalBufferedPacket();

	int mSessionID;
	char *mData;
	int mLength;
	bool mReliable;
	bool mCertain;
	bool mOrdered;
	unsigned int mBand;
};

const int kXPSessionUnknownID = -1;
const int kXPSessionBroadcastID = -2;

/* These are the internal message types for session messages */
const unsigned char kXPSessionPacketType_Unreliable				= 0;
const unsigned char kXPSessionPacketType_Reliable				= 1;
const unsigned char kXPSessionPacketType_Certain				= 2;
const unsigned char kXPSessionPacketType_Ordered				= 3;

/**
 * A network session class. This handles session nodes joining and leaving a session.<br>
 * This is a general purpose class for interfacing with the known transports and maintaining a stable interface between various connected nodes.
 * This class enables packets of data to be sent to a specific session node ID or to all session nodes as a broadcast. A session node is usually an individual computer or instance of this class.
 * In addition to this the class also offers a standard time interface that gives a synchronised shared between all connected nodes.
 * Sessions also have the option to make spider connections to other sessions directly. If enabled this means that sessions will attempt to use the shortest and quickest route for messages rather than relaying all messages through the master session. What this means in the real world is that a session that uses SetCanSpider(true) is advertising its self as being able to accept connections from other peers in the network topology. When a new session joins a spidering session will be told the new session has joined and advertise the peer connection service. If there is a fault with one of the session then the remaining sessions, if they have peer connections, will still be able to send messages around the gap in the network topology. Another session is then chosen to be the new overall session master.
 * An XPSession is used as the main basis for a ReplicaNet session.<br>
 * The terms "Reliable", "Unreliable" and "Certain" are references to the three packet delivery types used by ReplicaNet, XPSession and XPURL Transport classes.<br>
 * "Reliable" guarantees delivery of data in the same order it was sent.<br>
 * "Certain" guarantees delivery of data but the order the data is received may not be preserved.<br>
 * "Unreliable" does not guarantee delivery of data and the order of the data when it is received may not be preserved.<br>
 * In general it is a good idea to not use too many "Reliable" updates because this method is quite expensive in terms of network and memory performance. If one packet gets lost it will delay the delivery of all other Reliable data until that packet is resent. The better choice would be to use "Certain" because this is less expensive in terms of memory and network performance. Also if one Certain packet is lost the delivery of other Certain packets is not delayed. Generally use Reliable for those events that absolutely must occur in the same order they were sent.<br>
 */
class XPSession : public XPURL , public MessageHelper, public ThreadClass , private SysTime
{
public:

	/**
	 * Standard XPSession errors
	 */
	enum Error
	{
		kXPSession_EOK = 0,						/**< No error */
		kXPSession_EERROR = -1,					/**< A fatal error happened, like the connection broke and cannot be recovered */
		kXPSession_ETRANSPORT_CLOSED = -2,
		kXPSession_ETRANSPORT_ERROR = -3
	};

	/**
	 * The ctor for an XPSession
	 */
	XPSession();

	/**
	 * The dtor for an XPSession
	 * This cleans up everything for this class
	 */
	virtual ~XPSession();

	/**
	 * Allocates a new session to be used
	 * \return the new session that is allocated
	 */
	static XPSession *Allocate(void);

	/**
	 * Creates a new session using the default available protocols.
	 * <br>When this XPSession instance is disconnected from a session this XPSession instance should be deleted and a new instance allocated before attempting to create or join a session.
	 * \param name The name to give to this session. The maximum length of this string is 128 characters.
	 */
	void Create(const std::string name);

	/**
	 * Creates a new session using the supplied protocols
	 * <br>When this XPSession instance is disconnected from a session this XPSession instance should be deleted and a new instance allocated before attempting to create or join a session.
	 * \param name The name to give to this session
	 * \param protocols The list of protocols to use separated by commas. e.g. "UDP@,STREAMTCPIP@"
	 */
	void Create(const std::string name,const std::string protocols);

	/**
	 * Exports a URL that can be used to connect to this session in future.
	 * The format of the URL is as follows:<br>
	 * "SESSION://" the URL header.<br>
	 * Any number of exported Transport URLs separated with a '/'. Transport URLs are in the format "Transport type"@"address"<br>
	 * The standard XPSession brace block which contains: {SESSIONNAME=<session name>}<br>
	 * Any number of user defined brace blocks.<br>
	 * An example is as follows: SESSION://UDP@127.0.0.1:4000/{SESSIONNAME=Game}<br>
	 * \param shortAddress If this is true then only the short address for this session is exported. The short address only contains the address of the master listen transports, the long address also contains the listen addresses for the spider connections if they are available. By default the short address is exported.
	 * \return the URL for this session
	 */
	std::string ExportURL(const bool shortAddress = true);

	/**
	 * Tries to connect this session to the supplied URL
	 * <br>When this XPSession instance is disconnected from a session this XPSession instance should be deleted and a new instance allocated before attempting to create or join a session.
	 * \param url the url of the session to connect to
	 */
	void Join(const std::string url);

	/**
	 * Tries to find sessions using the default protocols
	 */
	void Find(void);

	/**
	 * Tries to find sessions using the listed protocols
	 * \param protocols The list of protocols to use separated by commas. e.g. "UDP@,STREAMTCPIP@"
	 */
	void Find(const std::string protocols);

	/**
	 * Enumerates any found sessions
	 * \return the URL of a session or a "" string
	 */
	std::string EnumerateFound(void);

	/**
	 * Gets the current channel number for this session
	 * \return the channel number
	 */
	int GetGameChannel(void);

	/**
	 * Sets the current channel number for this session. If no channel number is passed then TransportAnyGameChannel is used.
	 * \param channel The channel number to use.
	 */
	void SetGameChannel(const int channel = TransportAnyGameChannel);

	/**
	 * Gets the session unique ID
	 * \return the session unique ID
	 */
	int GetSessionID(void);

	/**
	 * This allows the user o check if the session is really stable.
	 * \return returns true if the session is stable
	 */
	bool IsStable(void);

	/**
	 * Gets the session status.
	 * A session error happens when the master session disconnects and a new master cannot be found.
	 * \return the session status will be either kXPSession_EOK or kXPSession_EERROR.
	 */
	Error GetStatus(void);

	/**
	 * This can be used to read extended information when XPSession encounters a session error.
	 * \return The error can be:<br>
	 * kXPSession_EOK for when there is no session error.<br>
	 * kXPSession_EERROR for undefined errors.<br>
	 * kXPSession_ETRANSPORT_CLOSED for when the session is disconnected or closed.<br>
	 * kXPSession_ETRANSPORT_ERROR when the session is abnormally terminated due to a transport timeout.
	 */
	Error GetSessionErrorReason(void);

	/**
	 * Sends data from this session to any other session via its ID or to all sessions by using XPSessionBroadcastID. If the node is in a pre-connect state and wants to send data to the master session then use kXPSessionUnknownID as the session ID to send to.<br>
	 * Data packets larger than 512 bytes can be sent and received correctly however these large packets may cause a performance hit as the large chunk of data is sent.
	 * Transport::GetMaxPacketSize() is used with the current connection instance to calculate if a packet needs to be sent as chunks.
	 * A sessionID will not receive any data sent to itself either by using its own sessionID or from a broadcast.
 	 * <br>The other functions DataSendToSessionXXX use this function
	 * \param sessionid the session id to send to or XPSessionBroadcastID.
	 * \param data the pointer to the data to send
	 * \param length the length of the data to send.
	 * \param reliable a flag to make this packet reliable, the default is true
	 * \param certain a flag to make this packet certain, the default is false
	 * \param ordered a flag to make this packet ordered, the default is false
	 * \param band The band to use for the underlying Transport when sending with the Reliable method.
	 */
	void DataSendToSession(const int sessionid,const char *data,const int length,const bool reliable = true,const bool certain = false,const bool ordered = false,const unsigned int band = 0);

	/**
	 * Sends data using the 'Unreliable' packet type from this session to any other session via it's ID or to all sessions by using XPSessionBroadcastID
	 * \param sessionid the session id to send to or XPSessionBroadcastID
	 * \param data the pointer to the data to send.
	 * \param length the length of the data to send.
	 */
	void DataSendToSessionUnreliable(const int sessionid,const char *data,const int length);

	/**
	 * Sends data using the 'Unreliable ordered' packet type from this session to any other session via it's ID or to all sessions by using XPSessionBroadcastID
	 * \param sessionid the session id to send to or XPSessionBroadcastID
	 * \param data the pointer to the data to send.
	 * \param length the length of the data to send.
	 * \param band The band to use for the underlying Transport when sending with the Reliable method.
	 */
	void DataSendToSessionOrdered(const int sessionid,const char *data,const int length,const unsigned int band = 0);

	/**
	 * Sends data using the 'Reliable' packet type from this session to any other session via it's ID or to all sessions by using XPSessionBroadcastID
	 * \param sessionid the session id to send to or XPSessionBroadcastID
	 * \param data the pointer to the data to send.
	 * \param length the length of the data to send.
	 * \param band The band to use for the underlying Transport when sending with the Reliable method.
	 */
	void DataSendToSessionReliable(const int sessionid,const char *data,const int length,const unsigned int band = 0);

	/**
	 * Sends data using the 'Certain' packet type from this session to any other session via it's ID or to all sessions by using XPSessionBroadcastID
	 * \param sessionid the session id to send to or XPSessionBroadcastID
	 * \param data the pointer to the data to send
	 * \param length the length of the data to send.
	 */
	void DataSendToSessionCertain(const int sessionid,const char *data,const int length);

	/**
	 * This allows the parameters of the next pending received data to be read. The data is left waiting to be read by DataReceive.
	 * \param fromsessionid the session id that sent this data.
	 * \param length the length of the data received.
	 * \param type a pointer that can receive the packet type. This will be kXPSessionPacketType_Unreliable, kXPSessionPacketType_Certain, kXPSessionPacketType_Reliable or kXPSessionPacketType_Ordered. This parameter can be null in which case the type is not filled in. The default value for this pointer is null.
	 * \return returns true if data was received. If false the entry parameters to this function are unchanged.
	 */
	bool DataReceivePeek(int *const fromsessionid,int *const length,unsigned char *const type = 0);

	/**
	 * Receives data and responds with what session sent this data.
	 * \param fromsessionid the session id that sent this data.
	 * \param data the buffer for the data.
	 * \param length the length of the data received.
	 * \param type a pointer that can receive the packet type. This will be kXPSessionPacketType_Unreliable, kXPSessionPacketType_Certain, kXPSessionPacketType_Reliable or kXPSessionPacketType_Ordered. This parameter can be null in which case the type is not filled in. The default value for this pointer is null.
	 * \return returns true if data was received. If false the entry parameters to this function are unchanged.
	 */
	bool DataReceive(int *const fromsessionid,char *const data,int *const length,unsigned char *const type = 0);

	/**
	 * Tests to see if this session is a master session
	 * \return returns true is the session is a master session, false if not
	 */
	bool IsMaster(void);

	/**
	 * Gets the session id that recently joined the network session
	 * \return the session id or kXPSessionUnknownID to indicate no more session ids changed
	 */
	int GetJoiner(void);

	/**
	 * Gets the session id that left the network session
	 * \return the session id or kXPSessionUnknownID to indicate no more session ids changed
	 */
	int GetLeaver(void);

	/**
	 * Gets the current system time of this ReplicaNet session synchronised with the time on the session master.
	 * \return the float time  1.0 units == 1 second
	 */
	SysTimeType GetTime(void);

	/**
	 * Gets the current local time. This time is local to the session and is not synchronised to the session master clock.
	 * \return the float time  1.0 units == 1 second
	 */
	SysTimeType GetLocalTime(void);

	/**
	 * Gets the current network send transfer rate in bytes per second
	 * \return bytes per second
	 */
	float GetNetworkSendRate(void);

	/**
	 * Gets the current network receive transfer rate in bytes per second
	 * \return bytes per second
	 */
	float GetNetworkReceiveRate(void);

	/**
	 * Gets the number of packets that are lost and lost and resent
	 * \return number of packets
	 */
	int GetNetworkPacketsLost(void);

	/**
	 * Gets the number of packets that have been rejected due to failing the checksum, decryption of decompression checks.
	 * \return number of packets
	 */
	int GetNetworkPacketsRejected(void);

	/**
	 * This polls XPSession. Normally an application will yield enough time during threads for XPSession to function.
	 * Sometimes an application that uses a lot of 3D graphics can lock out other threads for quite some time.
	 * In these cases you may call XPSession::Poll() to yield some time to XPSession to perform housekeeping.<br>
	 * Single threaded considerations:<br>
	 * When using the single threaded libraries this method should be called regularly. Unless ReplicaNet::Poll() is called in which case this Poll() method is already called.
	 */
	void Poll(void);

	/**
	 * This allows the user to determine if during a Poll() the layer below also gets called. For single threaded applications this function has no effect as the layer below always has the Poll() called. The default is true. <br>
	 * The layer below in this case is XPURL.
	 * \param enable Enable or disable layer below polling.
	 */
	void SetPollLayerBelow(bool enable = true);

	/**
	 * Allows the state of SetPollLayerBelow() to be read. If the library is always single threading then this function will always return true.
	 * \return the state of SetPollLayerBelow()
	 */
	bool GetPollLayerBelow(void);

	/**
	 * Sets the manual Poll() method for this class only.<br>
	 * For multi-threaded libraries this turns off the automatic polling method.<br>
	 * This allows an application to time exactly when polls are done.<br>
	 * The application must call Poll() on a regular basis. A good place is just before or after the frame update is done.
	 */
	void SetManualPoll(void);

	/**
	 * Sets the automatic Poll() method
	 * In multi-threading libraries this restarts the automatic polling thread so that an application does not need the call Poll()<br>
	 * In single-threading libraries this method has no effect and does not start a thread.<br>
	 * The default multi-threading library behaviour is to enable multi-threading automatic polls,<br>
	 * The default single-threading library behaviour is to use manual polls,<br>
	 */
	void SetAutomaticPoll(void);

	/**
	 * This allows the poll method to be set for the XPURL layer that is used by this session.
	 */
	void SetManualPollXPURL(void);

	/**
	 * This allows the poll method to be set for the XPURL layer that is used by this session.
	 */
	void SetAutomaticPollXPURL(void);


	/**
	 * This allows the user to enable or disable spidering of network connections
	 * \param canSpider Pass in true to enable spidering or false to disable spidering<br>
	 * The default is true
	 */
	void SetCanSpider(const bool canSpider = true);

	/**
	 * This allows the spidering state to be read
	 * /return the status of the spidering state
	 */
	bool GetCanSpider(void);

	/**
	 * This allows the session to set if it can become a master due to a fault being found from another session
	 * \param canBeMaster Pass in true to enable this session to become a master if needed.<br>
	 * The default is true<br>
	 * Currently a session can only become a master if it is enabled for spidering however.
	 */
	void SetCanBecomeMaster(const bool canBeMaster = true);

	/**
	 * Allows the state to be read
	 * \return the status of the becoming a master state
	 */
	bool GetCanBecomeMaster(void);

	/**
	 * This allows the latency to the master session to be read. This is the time taken for a packet to reach the master session. This is half round trip ping time.
	 * \return the latency to the master session
	 */
	float GetLatencyToMasterSession(void);

	/**
	 * This allows the latency to the sessionID to be read. This is the time taken for a packet to reach the sessionID. This is half round trip ping time.
	 * \return the latency to the sessionID.
	 */
	float GetLatencyToSessionID(const int sessionID);

	/**
	 * This allows the latency recalculation delay to be set. The default value is 1 second meaning there is a ping packet sent every second.
	 * \param seconds The number of seconds before latency is recalculated.
	 */
	void SetLatencyRecalculationDelay(const float seconds = 1.0f);

	/**
	 * Enables or disables encryption for this session
	 * \param enable set this to be true to enable encryption for packets sent from this session. The default is false
	 */
	void SetEncryption(const bool enable = false);

	/**
	 * Gets the status of the encryption flag for this session
	 */
	bool GetEncryption(void);


	/**
	 * This forces a disconnect from the supplied sessionID or if the sessionID is not supplied then all session nodes are disconnected. The disconnect is immediate and initiates the fault tolerant mechanism for XPSession and ReplicaNet on other connected session nodes. This means if the session was a master session node then the other session nodes will try to find a new master if this mechanism is enabled. In the case of a ReplicaNet session node having objects, these objects will attempt to use the fault recovery and migration to other nodes if it is enabled.
	 * \param sessionID The sessionID to disconnect from. The default is to disconnect from all sessionIDs.
	 */
	void Disconnect(const int sessionID = kXPSessionUnknownID);

	/**
	 * Allows the status of the pre-connect mechanism to be tested.
	 * \return When GetPreConnectStatus() returns true after this session node starts a join then this session node is in a pre-connect state.
	 */
	bool GetPreConnectStatus(void);

	/**
	 * Enables or disables the pre-connect mechanism for session nodes that are joining a session. When this mechanism is enabled the joining session node will be able to exchange packets with the master session node before the node properly joins the session. The function PreConnectHasFinished() can be called to signal that the node wishes to complete a connect to the session. This function should only be used before a session node is used to join the session. The function IsStable() will not return true until the pre-connect is finished.<br>
	 * The intended for this mechanism is as follows:<br>
	 * 1) SetPreConnect(true) and attempts to join a session. All calls to GetSessionID() will return kXPSessionUnknownID until after this session finishes the pre-connect and the master session transmits the correct sessionID.<br>
	 * 2) When GetPreConnectStatus() returns true after this session node starts a join then this session node is in a pre-connect state.<br>
	 * 3) This node will send one or more user defined packets that signal that the session wants to exchange certain pre-connect data to the master session using DataSendToSession() and session ID kXPSessionUnknownID. Using the reliable packet type is recommended but not mandatory.<br>
	 * 4) The master session will receive user data packets and detecting that pre-connect data has been received will in reply send a user data message to the effect that it has received this information.<br>
	 * 5) The joining node receives pre-connect data from the master session, verifies the data and either completes the pre-connect by calling PreConnectHasFinished() or uses Disconnect() to abort the connection.<br>
	 * \param enable Set this to be true to enable the pre-connect mechanism for this session node. If false then the session node will discard all pre-connect packets and the preconnection mechanism is assumed to be disabled. The default is false.
	 */
	void SetPreConnect(const bool enable = false);

	/**
	 * Gets the status of the pre-connect mechanism for this session node.
	 * \return Returns true if the pre-connect mechanism is enabled.
	 */
	bool GetPreConnect(void);

	/**
	 * If the pre-connect mechanism is enabled then this function must be called by the joining session node when packets have been exchanged to the satisfaction of the joining session node.
	 */
	void PreConnectHasFinished(void);

	/**
	 * Enables or disables the automatic packet compression test. If enabled XPSession will test each packet sent to see if it can be compressed and if there is a space saving then the packet is compressed before sending.
	 * \param enable Enable or disable automatic packet compression. The default is false to disable automatic packet compression.
	 */
	void SetAutomaticPacketCompression(const bool enable = false);

	/**
	 * Gets the value configured by SetAutomaticPacketCompression()
	 * \return Returns true if the automatic packet compression is enabled.
	 */
	bool GetAutomaticPacketCompression(void);

	/**
	 * Gets the compression statistics totals for packets sent from this session node.
	 * \param before The total number of bytes before compression. This can be null.
	 * \param after The total number of bytes after compression. This can be null.
	 * \param reset If true, after returning the information to the caller both totals are reset back to zero. The default value is false.
	 */
	void GetCompressionStatistics(int *const before=0,int *const after=0,const bool reset = false);

	/**
	 * Gets the merged packet statistics totals for connections managed by this session node.
	 * <br>Note: This function can take some time to execute when there are hundreds of connected users.
	 * \param sent The total number of merged sent packets. This can be null.
	 * \param received The total number of merged sent packets. This can be null.
	 * \param reset If true, after returning the information to the caller both totals are reset back to zero. The default value is false.
	 */
	void GetMergedStatistics(int *const sent=0,int *const received=0,const bool reset = false);

	/**
	 * Gets the master session unique ID. If the session is not connected then the return value is kXPSessionUnknownID.
	 * \return the master session unique ID
	 */
	int GetMasterSessionID(void);

	/**
	 * Attempts to resolve the URL of the session unique ID from the point of view of this session node.
	 * If the session unique ID is not known to the session then the result is a null string.
	 * If the session unique ID is not directly connected to this node, for example the spider connection was broken, then this function is not guaranteed to produce a URL and can return a null string.
	 * \return The URL in the same format supplied by Transport::GetPeerURL()
	 */
	std::string GetURLFromSessionID(const int sessionID);

	/**
	 * Sets the client only flag for this session node. The default value is false which means this node will get joiner and leaver messages. If this is set to be true then this node will only get joiner/leaver messages for spider nodes or nodes that can become master nodes. This must not be changed after the session is used for joining a session.
	 * Setting this to option to be true also automatically sets SetCanBecomeMaster() and SetCanSpider() to be false.
	 * \param isClient The default value is false.
	 */
	void SetClientOnly(const bool isClient = false);

	/**
	 * Gets the client only configuration set by SetClientOnly().
	 * \return The configuration set by SetClientOnly().
	 */
	bool GetClientOnly(void);

	/**
	 * Attempts to read the configured bandwidth for a route to a specific session ID. If the session ID is not directly connected to this node then this function will return false and the contents of the output and input pointers will be left unchanged.
	 * If this node can supply bandwidth information about a session ID then this function will return true and the bandwidth, output, input pointers will contain the values as used by SetBandwidthLimit(), SetMaximumOutputBandwidth() and SetMaximumInputBandwidth(). Note that output and input bandwidth will appear to be exchanged because the output of one end of transport connection is really the input of the other end of the connection.
	 * <br>Note: This function can take some time to execute when there are hundreds of connected users.
	 * \param sessionID The session ID of the required node.
	 * \param bandwidth A pointer to accept the Transport::Bandwidth value, this can be null.
	 * \param output A pointer to accept the output bandwidth value, this can be null.
	 * \param input A pointer to accept the input bandwidth value, this can be null.
	 * \return Success returns true, failure returns false.
	 */
	bool GetBandwidthFromSessionID(const int sessionID,Transport::Bandwidth *const bandwidth,int *const output,int *const input);

	/**
	 * Enables or disables any further connection attempts to this session.
	 * \param allow Set this to be true to allow connections to this session. False will stop any further connection attempts to this session and the connecting session will get kXPSession_ETRANSPORT_CLOSED from GetSessionErrorReason().
	 */
	void SetAllowConnections(const bool allow = true);

	/**
	 * Returns the value configured by SetAllowConnections()
	 * \retrurn the value.
	 */
	bool GetAllowConnections(void);

	/**
	 * Enables or disables buffering for this session node. With buffering enabled extra memory is used to improve multi-threaded performance when this node is used by another thread.
	 * \param enable The default is false which disables extra buffering.
	 */
	void SetNodeBuffers(const bool enable = false);

	/**
	 * Returns the value configured by SetNodeBuffers()
	 * \return The value.
	 */
	bool GetNodeBuffers(void);

	typedef int GetSessionIDCallback(void *context);

	/**
	 * Registers a callback for the user to create their own sessionIDs. The handle can be used by the user to store context specific data and is passed to the registered callback function.
	 * The user must guarantee that the calculated sessionID is greater than 0 and less than 32768 and is unique for the entire network session.
	 */
	void RegisterSessionIDCallback(GetSessionIDCallback *callback = 0,void *context=0);


	void SetProxyMode(const bool enable = false);

	bool GetProxyMode(void);

	/**
	 * Sets the maximum size of data that can be sent or received.
	 * If a session sends more data than the recipient is willing to receive then the recipient can decide to disconnect the source session connection.
	 */
	void SetMaximumDataSize(const int bytes = 64*1024);
	int GetMaximumDataSize(void) const;

	/**
	 * Sets the compression level to use when compressing normal session packets.
	 */
	void SetPacketCompressionLevel(const int level = 10);
	/**
	 * Sets the compression level to use when compressing large session packets that are split into chunks.
	 */
	void SetLargePacketCompressionLevel(const int level = 5);

	int GetPacketCompressionLevel(void) const;
	int GetLargePacketCompressionLevel(void) const;

	/**
	 * Instead of using discrete data compression for each data packet sent using XPSession, use the Transport packet compression instead. This can result in higher compression ratios.
	 * See Transport::SetTransparentPacketCompression() for more details.
	 * Note: If this compression method is enabled then use SetAutomaticPacketCompression(false) and SetEncryption(false) for best performance.
	 */
	void SetTransparentPacketCompression(const int level = 0);

	/**
	 * Allows the setting for SetTransparentPacketCompression() to be read.
	 * \return The setting for SetTransparentPacketCompression()
	 */
	int GetTransparentPacketCompression(void);

private:
struct Route;
friend struct Route;
// Variables in initialiser
	int mGameChannel;
	int mSessionID;
	Error mStatus;
	bool mIsMaster;
	int mMasterSessionID;		/**< A reference number to the master session ID */
	Thread *mThread;
	int mBufferLen;
	char *mMessageBufferIn;
	char *mMessageBufferOut;
	float mCurrentSendRate;
	float mCurrentRecvRate;
	SysTimeType mLastRateCalcTime;
	bool mCanSpider;
	bool mCanBeMaster;
	bool mCompletelyStable;
	bool mTryingJoin;
	SysTimeType startPollTime;
	SysTimeType mLocalTimeStartPollTime;
	int mPotentialMasterSessionID;	/**< When a Route has an error the potential master ID is calculated and stored here */
	int mNoticedHighestSessionID;
	bool mManualPoll;
	bool mEncryptionOn;
	bool mPollLayerBelow;
	bool mEnablePreConnect;


// End of initialiser variables
	std::vector<Transport *> mMasterListens;	/**< The master listen transports */
	std::vector<Transport *> mListens;			/**< The normal listen transport */
	std::list<Route *> mConnections;			/**< This session connections */
	std::multimap<int,Route *> mConnectionsBySessionID;	/**< For faster lookup with Route::mTargetSessionID */
	std::vector<Transport *> mFindTransports;	/**< This session find transports */

	std::string mSessionName;

	int mNextSessionID;							/** This can be sequential, so that the variable really does hold the next sessionId or if mGetSessionIDCallback is used then is holds the sessionID of the last connection */



	struct WaitingPacket
	{
		WaitingPacket()
		{
			mFromSession = kXPSessionUnknownID;
			mPacket = 0;
			mSize = 0;
			mPacketType = kXPSessionPacketType_Unreliable;
		}

		virtual ~WaitingPacket()
		{
			if (mPacket)
			{
				free (mPacket);
				mPacket = 0;
			}
		}

		int mFromSession;
		int mSize;
		char *mPacket;
		unsigned char mPacketType;
	};

	std::list<WaitingPacket *> mWaitingPackets;

	std::list<int> mJoiners;
	std::list<int> mLeavers;

	// When the session master disconnects then any routes that have errors are pushed on to the potential leaver list
	std::list<int> mPotentialLeavers;


	SysTimeType mZeroTime;
	float mZeroTimeDelta;
	SysTimeType mLastTimeCorrectionTime;
	SysTimeType mTargetZeroTime;


	bool mBreakoutDone;
	bool mDontDoBreakOut;

	float mLatencyRecalculationDelay;

	bool mInPreConnect;
	bool mPreConnectSignalDone;
	int mPreConnectSendToID;

	bool mCreateOrJoinDone;

	bool mEnableAutomaticCompression;
	int mCompressionBefore;
	int mCompressionAfter;

	bool mIsClientOnly;
	bool mTransportDisconnected;
	bool mTransportError;

	bool mAllowConnections;

	int mNetworkPacketsLost;
	int mNetworkPacketsRejected;

	bool mEnableExtraBuffering;
	void HandleWaitingToSendPackets(void);
	MutexClass mMutexSentPackets;
	int mNumWaitingToSend;
	std::list<XPSessionInternalBufferedPacket*> mWaitingToSend;
	MutexClass mMutexRecvPackets;
	MutexClass mMutexJoinerList;
	MutexClass mMutexLeaverList;
	// A mutex for locking misc values that are buffered below
	MutexClass mMutexMiscValues;

	// Return values that are locked against mMutexMiscValues
	std::string mReturnExportURLTrue;
	std::string mReturnExportURLFalse;
	std::map<int,std::string> mBufferedURLsBySessionID;
	std::map<int,float> mBufferedLatencyBySessionID;

	XPSession::GetSessionIDCallback *mGetSessionIDCallback;
	void *mGetSessionIDCallbackContext;

	bool mProxyMode;
	bool mMasterIsProxyMode;

	// Private functions that mutex lock the main class
	std::string BufferedExportURL(const bool shortAddress);
	int BufferedGetNetworkPacketsLost(void);

	void MutateToMaster(const int nextsessionid,const Route *route);

	// This just exports the transport URLs and not the session name. Used by the spider to code.
	std::string ExportURLInternal(const bool ShortAddress = true);

	std::string GetSessionString(void);

	Route *FindRouteForSessionIDFromMap(const int sessionID);

	/**
	 * Initialises the class
	 * \return returns false if initialisation failed
	 */
	bool Initialise(void);


	void DataSendToSessionPrivate(const int sessionid,const char *data,const int length,const bool reliable = true,const bool certain = false,const bool ordered = false,const unsigned int band = 0);
	void DataPacketSendPrivate(const int finalDestID,const char *data,const int length,const bool reliable,const bool certain,const bool ordered,const unsigned int band,Route *route);

	void InternalSendMessage(Route *routed, const SessionPacket_Header &message,const int length);
	bool ParsePacket(Route *const route,int length);

	std::string AppendSessionInfo(std::string instr);


	void HandleTransportDiscovery(Transport *const trans);
	int ThreadEntry(void);

	// Compress and encrypt session packets are two very similar functions.
	// They both act on the current message buffer only and modify the message header.
	void CompressSessionPacket(SessionPacket_Header &message);
	void EncryptSessionPacket(SessionPacket_Header &message);

	void AddJoiner(const int sessionid);
	void AddLeaver(const int sessionid);

	void CreateBufferedURL(void);

	int mMaximumDataSize;
	int mPacketCompressionLevel;
	int mLargePacketCompressionLevel;

	int mTransparentPacketCompression;
};

} // namespace RNReplicaNet

#endif
