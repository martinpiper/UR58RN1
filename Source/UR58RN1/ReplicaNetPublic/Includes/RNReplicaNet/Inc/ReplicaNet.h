/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __REPLICANET_H__
#define __REPLICANET_H__

#define _REPLICANET_VERSION 7300

#include <string>

#include "RNXPURL/Inc/Transport.h"
#include "RNPlatform/Inc/SysTime.h"

namespace RNReplicaNet
{

class ReplicaObject;
class ReplicaNetPrivate;
struct s_RO_RegistryBlock;
struct DataBlockReason;

class XPSession;
class MessageHelper;
class DynamicMessageHelper;

const int kReplicaNetUnknownUniqueID = -1;
const int kReplicaNetBroadcastID = -2;
const int kReplicaNetRecordID = -3;

/**
 * The main class for ReplicaNet.
 * This class handles everything to do with the management of all ReplicaObject types that are allocated to a network session.<br>
 * ReplicaNet uses XPSession to manage a network session which includes session migration and client-server or peer-to-peer network topology.<br>
 * Each instance of ReplicaNet can be configured depending on the intended use on a per-machine basis.
 * As an example a Massively Multiplayer Online (MMO) server node could use:<br>
\verbatim
SetLoadBalancing(true);		// This is optional and should only be used if you expect your server objects to be moved between other nodes that use this flag.
SetCanAcceptObjects(true);	// In the event of a server node failure this node can accept fault tolerant objects.
SetCanBecomeMaster(true);	// In the event of the master server node failure this node is capable of becoming the new master.
SetCanSpider(true);			// This node will advertise a connection that can be used to communicate directly to other nodes.
// Adding packet buffering using the TransportConfig class will reduce the number of packets sent over the wire.
TransportConfig::SetPacketBufferTime(1.0f / 50.0f);
TransportConfig::SetPacketBufferEnable(true);
// Using multi-threaded libraries does help performance for a server that is going to have a high load and automatically uses multiple CPU cores or Hyper-Threads if the hardware/OS support is available.
SetAutomaticPoll();			// Allow automatic polling of ReplicaNet.
SetAutomaticPollXPSession();// Allow automatic polling of XPSession.
SetAutomaticPollXPURL();	// Allow automatic polling of XPURL and Transports.
SetPollLayerBelow(false);	// Separates the ReplicaNet thread from the XPSession thread.
SetPollLayerBelowForXPSession(false);	// Separates the XPSession thread from the XPURL threads.
SetNodeBuffers(true);		// This uses extra memory for low-latency cross thread buffers to improve threading performance.
\endverbatim
Spidering can also be set to be on for server nodes and this this helps to distribute network bandwidth.<br>

For the clients:<br>
\verbatim
SetLoadBalancing(false);
SetCanAcceptObjects(false);
SetCanBecomeMaster(false);
SetCanSpider(false);
SetClientOnly(true);		// This lowers bandwidth to this node and ensures only this node sends object updates to server nodes.
// Adding packet buffering using the TransportConfig class will reduce the number of packets sent over the wire.
TransportConfig::SetPacketBufferTime(1.0f / 50.0f);
TransportConfig::SetPacketBufferEnable(true);
\endverbatim
Note that all the clients are not going to have peer connections to other clients. This is what you want for a MMO.<br>

Or for a traditional peer-to-peer LAN/WAN type of game with host and object migration:<br>
\verbatim
SetLoadBalancing(false);
SetCanAcceptObjects(true);
SetCanBecomeMaster(true);
SetCanSpider(true);
// Adding packet buffering using the TransportConfig class will reduce the number of packets sent over the wire.
TransportConfig::SetPacketBufferTime(1.0f / 50.0f);
TransportConfig::SetPacketBufferEnable(true);
\endverbatim

These options are explained below in the documentation.<br>

ReplicaNet can also send and receive raw data packets from the session for those applications that do not always want to use the data driven ReplicaObject methods of propagation.<br>
 * The terms "Reliable", "Unreliable", "Unreliable ordered" and "Certain" are references to the four packet delivery types used by ReplicaNet, XPSession and XPURL Transport classes.<br>
 * "Reliable" guarantees delivery of data in the same order it was sent.<br>
 * "Certain" guarantees delivery of data but the order the data is received may not be preserved.<br>
 * "Unreliable" does not guarantee delivery of data and the order of the data when it is received may not be preserved.<br>
 * "Unreliable ordered" does not guarantee delivery of data but does guarantee the order of the data when it is received.<br>
 * In general it is a good idea to not use too many "Reliable" updates because this method is quite expensive in terms of network and memory performance. If one packet gets lost it will delay the delivery of all other Reliable data until that packet is resent. The better choice would be to use "Certain" because this is less expensive in terms of memory and network performance. Also if one Certain packet is lost the delivery of other Certain packets is not delayed. Generally use Reliable for those events that absolutely must occur in the same order they were sent.<br>
 * For most ReplicaObject updates use the Certain update type because generally it doesn't matter if an object is sometimes created in a different order to other objects. By default ReplicaObject uses the Reliable method to give the most reliable performance for easier first-pass testing of new networked software. This means network performance can be improved by using the ROL code:
\verbatim
object ObjectFoo
{
	networking
	{
	... Some member variables ...
	}

	// Set Certain updates for this object.
	Certain();
}
\endverbatim
The default for ReplicaObject class member variables (the DataBlock derived classes) is to use Unreliable packets. For example it generally doesn't matter if a position update packet is lost because another update will be transmitted at some point in the near future. Remember that each network shared ReplicaObject class member variable using a DataBlock can have a different packet update type.
 */
class ReplicaNet
{
public:
	/**
	 * Standard ReplicaNet errors
	 */
	enum Error
	{
		kReplicaNet_EOK = 0,						/**< No error. */
		kReplicaNet_EERROR = -1,					/**< A fatal error happened, like the connection broke and cannot be recovered. */
		kReplicaNet_ETRANSPORT_CLOSED = -2,
		kReplicaNet_ETRANSPORT_ERROR = -3,
		kReplicaNet_EEND_RECORDING = -4
	};

	enum PacketType
	{
		kPacket_Unreliable = 0,
		kPacket_Reliable = 1,
		kPacket_Certain = 2,
		kPacket_Ordered = 3
	};

	/**
	 * The ctor performs some basic initialisation
	 */
	ReplicaNet();

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~ReplicaNet();

	/**
	 * This registers all of the objects used in the application to the ReplicaNet class. This is used by ReplicaNet when a new node is allocated.
	 */
	virtual void PreRegisterReplicas(void);

	/**
	 * Static function to return the current ReplicaNet class that is being used
	 * \return returns the current ReplcaNet class in use
	 */
	static ReplicaNet *GetCurrentReplicaNet(void);

	/**
	 * Static function to set the current ReplicaNet class that is being used
	 * \param replicanet the ReplicaNet class pointer
	 */
	static void SetCurrentReplicaNet(ReplicaNet *const replicanet);

	/**
	 * Gets the session ID from the session associated with the ReplicaNet class
	 */
	int GetSessionID(void);

	/**
	 * Gets the current channel number
	 * \return the channel number
	 */
	int GetGameChannel(void);

	/**
	 * Sets the current channel number. If no channel number is passed then TransportAnyGameChannel is used.
	 * \param channel The channel number to use.
	 */
	void SetGameChannel(const int channel = TransportAnyGameChannel);

	/**
	 * Creates a new session using the default available protocols.
	 * <br>When this ReplicaNet instance is disconnected from a session this ReplicaNet instance should be deleted and a new instance allocated before attempting to create or join a session.
	 * \param name The name to give to the ReplicaNet session. The maximum length of this string is 128 characters.
	 */
	void SessionCreate(const std::string name);

	/**
	 * Creates a new session using the supplied protocols.
	 * <br>When this ReplicaNet instance is disconnected from a session this ReplicaNet instance should be deleted and a new instance allocated before attempting to create or join a session.
	 * \param name The name to give to the ReplicaNet session
	 * \param protocols The list of protocols to use separated by commas. e.g. "UDP@,STREAMTCPIP@"
	 */
	void SessionCreate(const std::string name,const std::string protocols);

	/**
	 * Exports a URL that can be used to connect to this session in future. See XPSession::ExportURL().
	 * \return the full URL for the ReplicaNet session
	 */
	std::string SessionExportURL(void);

	/**
	 * Tries to connect this session to the supplied URL
	 * <br>When this ReplicaNet instance is disconnected from a session this ReplicaNet instance should be deleted and a new instance allocated before attempting to create or join a session.
	 * \param url the url of the ReplicaNet session to connect to
	 */
	void SessionJoin(const std::string url);

	/**
	 * Tries to find sessions using the default protocols
	 */
	void SessionFind(void);

	/**
	 * Tries to find sessions using the listed protocols
	 * \param protocols The list of protocols to use separated by commas. e.g. "UDP@,STREAMTCPIP@"
	 */
	void SessionFind(const std::string protocols);

	/**
	 * Enumerates any found sessions
	 * \return the URL of a session or a "" string
	 */
	std::string SessionEnumerateFound(void);

	/**
	 * This function locks the object lists so that the user application can guarantee that objects won't be created or deleted while accessing the objects created by ReplicaNet
	 */
	void LockObjects(void);

	/**
	 * This unlocks the object lists from a previous LockObjects() call
	 */
	void UnLockObjects(void);

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
	 * Gets the number of packets lost so far by the network layer.
	 * \return the number of packets lost.
	 */
	int GetNetworkPacketsLost(void);

	/**
	 * Gets the number of packets that have been rejected due to failing the checksum, decryption of decompression checks.
	 * \return number of packets
	 */
	int GetNetworkPacketsRejected(void);

	/**
	 * Gets the current system time of this ReplicaNet session synchronised with the time on the session master.
	 * This function is virtual allowing the application to provide their own source for time.
	 * If providing a custom source of time and recording a session or playing a session recording then care must be taken to provide proper synchronisation of time.
	 * \return the float time  1.0 units == 1 second
	 */
	virtual SysTimeType GetTime(void);

	/**
	 * Gets the current local time. This time is local to the session and is not synchronised to the session master clock.
	 * This function is virtual allowing the application to provide their own source for time.
	 * If providing a custom source of time and recording a session or playing a session recording then care must be taken to provide proper synchronisation of time.
	 * \return the float time  1.0 units == 1 second
	 */
	virtual SysTimeType GetLocalTime(void);

	/**
	 * Sets a ReplicaObject as the observer for this ReplicaNet session. Any ReplicaObject master or replica can be used as an observer. If a session has an object set as an observer but the observed object has not arrived yet the session will not receive any distance based propagated objects. Only when the observed object arrives will distance based objects be propagated. Using the Reliable or Certain object update type will guarantee delivery of the object update. The object must have been published, registered on the network session, (using ReplicaObject::Publish()) before being observed, an assert will be generated in debug builds.
	 * \param object the object to use as an observer. Pass in NULL or omit the parameter to unset the observer.
	 */
	void SetObserver(ReplicaObject *const object = 0);

	/**
	 * Gets the current observer ReplicaObject.
	 * \return returns the point to the object that is the observer, or NULL if there is no observer.
	 */
	ReplicaObject *GetObserver(void);

	/**
	 * This polls ReplicaNet. Normally an application will yield enough time during threads for ReplicaNet to function.
	 * Sometimes an application that uses a lot of 3D graphics can lock out other threads for quite some time.
	 * In these cases you may call ReplicaNet::Poll() to yield some time to ReplicaNet to perform housekeeping.
	 * This method also calls the XPSession::Poll() and XPURL::Poll() methods.<br>
	 * Single threaded considerations:<br>
	 * When using the single threaded libraries this method should be called regularly.
	 */
	void Poll(void);

	/**
	 * This allows the user to determine if during a Poll() the layer below also gets called. For single threaded applications this function has no effect as the layer below always has the Poll() called. The default is true. <br>
	 * The layer below in this case is XPSession.
	 * \param enable Enable or disable layer below polling.
	 */
	void SetPollLayerBelow(bool enable = true);

	/**
	 * Allows the state of SetPollLayerBelow() to be read. If the library is always single threading then this function will always return true.
	 * \return the state of SetPollLayerBelow()
	 */
	bool GetPollLayerBelow(void);

	/**
	 * This calls XPSession::SetPollLayerBelow(). Please see the documentation in XPSession for more information. The default is true. <br>
	 * \param enable Enable or disable layer below polling.
	 */
	void SetPollLayerBelowForXPSession(bool enable = true);

	/**
	 * This calls XPSession::GetPollLayerBelow(). Please see the documentation in XPSession for more information.
	 * \return the state of XPSession::GetPollLayerBelow()
	 */
	bool GetPollLayerBelowForXPSession(void);

	/**
	 * Sets the manual Poll() method for this class only.<br>
	 * For multi-threaded libraries this turns off the automatic polling method.<br>
	 * This allows an application to time exactly when polls are done.<br>
	 * Doing this allows the extrapolation and prediction routines to calculate a more reliable set of data to send to other sessions.<br>
	 * The application must call Poll() on a regular basis. A good place is just before or after the frame update is done.
	 */
	void SetManualPoll(void);

	/**
	 * Sets the automatic Poll() method
	 * In multi-threading libraries this restarts the automatic polling thread so that an application does not need the call Poll()<br>
	 * In single-threading libraries this method has no effect and does not start a thread.<br>
	 * The default multi-threading library behavior is to enable multi-threading automatic polls,<br>
	 * The default single-threading library behavior is to use manual polls,<br>
	 */
	void SetAutomaticPoll(void);

	/**
	 * This allows the poll method to be set for the XPSession layer that is used by this session.
	 */
	void SetManualPollXPSession(void);

	/**
	 * This allows the poll method to be set for the XPSession layer that is used by this session.
	 */
	void SetAutomaticPollXPSession(void);

	/**
	 * This allows the poll method to be set for the XPURL layer that is used by this session.
	 */
	void SetManualPollXPURL(void);

	/**
	 * This allows the poll method to be set for the XPURL layer that is used by this session.
	 */
	void SetAutomaticPollXPURL(void);

	/**
	 * This tries to change the ownership on a ReplicaObject to another session.
	 * This only works if the ReplicaObject is a Master object and approves migration.
	 * \param object the pointer to the ReplicaObject
	 * \param sessionID the target sessionID for this object to migrate to
	 */
	void GiveOwnership(ReplicaObject *const object,const int sessionID);

	/**
	 * This requests a change of the ownership on a ReplicaObject to be the calling session.
	 * This only works if the ReplicaObject is a Replica object and approves migration.
	 * \param object the pointer to the ReplicaObject
	 */
	void RequestOwnership(ReplicaObject *const object);

	/**
	 * This allows the user to enable or disable spidering of network connections
	 * \param canSpider Pass in true to enable spidering or false to disable spidering
	 * The default is true
	 */
	void SetCanSpider(const bool canSpider = true);

	/**
	 * This allows the spidering state to be read
	 * /return the status of the spidering state
	 */
	bool GetCanSpider(void);

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
	 * This allows the user to check if the session is really stable.
	 * \return returns true if the session is stable
	 */
	bool IsStable(void);

	/**
	 * Returns true if this ReplicaNet session is the master session
	 * \return true if this session is the master
	 */
	bool IsMaster(void);

	/**
	 * This allows the session to set if it can become a master due to a fault being found from another session
	 * \param canBeMaster Pass in true to enable this session to become a master if needed.<br>
	 * The default is true<br>
	 * Once a session is created or joined this parameter cannot be changed.<br>
	 * Currently a session can only become a master if it is enabled for spidering however.
	 */
	void SetCanBecomeMaster(const bool canBeMaster = true);

	/**
	 * Allows the state to be read
	 * \return the status of the becoming a master state
	 */
	bool GetCanBecomeMaster(void);

	/**
	 * Gets the session status.
	 * \return the session status will be either kReplicaNet_EOK or kReplicaNet_EERROR.
	 */
	Error GetStatus(void);

	/**
	 * This can be used to read extended information when ReplicaNet encounters a session error.
	 * \param error The error can be:<br>
	 * kReplicaNet_EOK for when there is no session error.<br>
	 * kReplicaNet_EERROR for undefined errors.<br>
	 * kReplicaNet_ETRANSPORT_CLOSED for when the session is disconnected or closed.<br>
	 * kReplicaNet_ETRANSPORT_ERROR when the session is abnormally terminated due to a transport timeout.
	 */
	Error GetSessionErrorReason(void);

	enum OnError
	{
		kMigrateObjects = 0,
		kDeleteObjects,
		kLeaveObjects,
		kMigrateObjectsToNewMaster
	};

	/**
	 * If a session error happens then this enables the user to decide if all ReplicaObjects should be tested for migration to this session.
	 * A session error happens when the master session disconnects and a new master cannot be found.
	 * \param enable Set this to be true if all objects should be tested for migration to this session on an error condition<br>
	 * The default is kMigrateObjects which takes all the objects from other sessions and converts them to be master objects if possible.<br>
	 * kMigrateObjectsToNewMaster Causes objects that belong to other sessions to migrate to the new master if the new master uses SetCanAcceptObjects(true).<br>
	 * kDeleteObjects Causes objects that belong to other sessions to be deleted.<br>
	 * kLeaveObjects Causes objects that belong to other sessions to be unchanged and no migration checks are used.<br>
	 * Once a session is created or joined this parameter cannot be changed.
	 */
	void SetOnSessionError(const OnError whatToDo = kMigrateObjects);

	/**
	 * Allows the status of SetOnSessionErrorMigrateObjects() to be read
	 * \return the status of SetOnSessionErrorMigrateObjects()
	 */
	OnError GetOnSessionErrorMigrateObjects(void);

	/**
	 * Allows the user to set if this ReplicaNet session can accept objects via migration
	 * \param accept Set this to be true if this session can accept objects through migration<br>
	 * However if this session becomes a master and no other sessions can accept objects then this session uses the rule from SetOnSessionError() and flags a session error if objects are left when using kLeaveObjects.
	 * The default value is true<br>
	 * Once a session is created or joined this parameter cannot be changed.
	 */
	void SetCanAcceptObjects(bool accept = true);

	/**
	 * Allows the status of SetCanAcceptObjects() to be read
	 * \return the status of SetCanAcceptObjects()
	 */
	bool GetCanAcceptObjects(void);

	/**
	 * Enables or disables encryption for this session
	 * \param enable set this to be true to enable encryption for data sent from this session. The default is false
	 */
	void SetEncryption(const bool enable = false);

	/**
	 * Gets the status of the encryption flag for this session
	 * \return true if encryption is enabled
	 */
	bool GetEncryption(void);

	/**
	 * Enables or disables load balancing for this ReplicaNet session.
	 * \param enable set this to true to enable load balancing. The default is false
	 * Once a session is created or joined this parameter cannot be changed.
	 */
	void SetLoadBalancing(const bool enable = false);

	/**
	 * Gets the status of the load balancing flag
	 * \return true if load balancing is enabled
	 */
	bool GetLoadBalancing(void);

	/**
	 * SetMaximumLoad() sets the maximum load that the current session ID will try to have. This is specifically for when ReplicaNet::SetLoadBalancing(true) is used. Each object can have a 'load' value set by using ReplicaObject::SetLoadBalancing(true) and also setting the load value by using ReplicaObject::SetLoadScore(float). When the total load for the master objects for a session ID exceeds the SetMaximumLoad() value then the session tries to migrate objects to a session ID that has a smaller load. This allows a system to be scaled such that if the server cluster was becoming full then another machine in the cluster could be started and some objects would be migrated to the new machine.
	 * The definition and scale of load "units" is left to the user to define.
	 * \param load the load in load "units"
	 * Once a session is created or joined this parameter cannot be changed.
	 */
	void SetMaximumLoad(float load = 0.0f);

	/**
	 * Gets the maximum load set by SetMaximumLoad() or the maximum load for a supplied session ID if one is supplied
	 * \param sessionID the session ID to return the load value for
	 * \return the load setting
	 */
	float GetMaximumLoad(int sessionID = kReplicaNetUnknownUniqueID);

	/**
	 * Gets the total load for a session ID. If the session ID is set to be kReplicaNetUnknownUniqueID then the load for the current session is returned
	 * \param sessionID the session ID to return the load value for
	 * \return the total load for a session ID
	 */
	float GetTotalLoad(int sessionID = kReplicaNetUnknownUniqueID);

	/**
	 * Starts iterating through all the allocated objects, effectively locking the list until ObjectListFinishIterate() is called.
	 */
	void ObjectListBeginIterate(void);

	/**
	 * Unlocks the iterated list of objects.
	 */
	void ObjectListFinishIterate(void);

	/**
	 * Iterates through the object list.
	 * \return the pointer to the next object known to this ReplicaNet session or returns null if there are no more objects.
	 */
	ReplicaObject *ObjectListIterate(void);

	/**
	 * Find a ReplicaObject using its sessionID and uniqueID.
	 * \return the pointer to the object known to this ReplicaNet session if it is found or returns null if it was not found.
	 */
	ReplicaObject *ObjectListFind(const int sessionID,const int uniqueID);

	/**
	 * This virtual callback method allows a derived ReplicaNet class to be notified when a session ID joins the network session.<br>
	 * Sessions should not be disconnected by the user during this callback because all ReplicaObjects will see the sessionID join and then very quickly leave.
	 * To be able to disconnect potentially joining sessions use the SetPreConnect() mechanism.
	 * This method is called by ReplicaNet before all of the internal routines are run when a session joins.
	 */
	virtual void JoinerSessionIDPre(const int sessionID);

	/**
	 * This virtual callback method allows a derived ReplicaNet class to be notified when a session ID joins the network session.<br>
	 * Sessions should not be disconnected by the user during this callback because all ReplicaObjects will see the sessionID join and then very quickly leave.
	 * To be able to disconnect potentially joining sessions use the SetPreConnect() mechanism.
	 * This method is called by ReplicaNet after all of the internal routines are run when a session joins.
	 */
	virtual void JoinerSessionIDPost(const int sessionID);

	/**
	 * This virtual callback method allows a derived ReplicaNet class to be notified when a session ID leaves the network session.<br>
	 * This method is called by ReplicaNet before any of the internal routines are run when a session leaves.
	 */
	virtual void LeaverSessionIDPre(const int sessionID);

	/**
	 * This virtual callback method allows a derived ReplicaNet class to be notified when a session ID leaves the network session.<br>
	 * This method is called by ReplicaNet after any of the internal routines are run when a session leaves.
	 */
	virtual void LeaverSessionIDPost(const int sessionID);



	/**
	 * This forces a disconnect from the supplied sessionID or if the sessionID is not supplied then all session nodes are disconnected. The disconnect is immediate and initiates the fault tolerant mechanism for XPSession and ReplicaNet on other connected session nodes. This means if the session was a master session node then the other session nodes will try to find a new master if this mechanism is enabled. In the case of a ReplicaNet session node having objects, these objects will attempt to use the fault recovery and migration if it is enabled.<br>
	 * \param sessionID The sessionID to disconnect from. The default is to disconnect from all sessionIDs.
	 */
	void Disconnect(const int sessionID = kReplicaNetUnknownUniqueID);

	/**
	 * Allows the status of the pre-connect mechanism to be tested.
	 * \return When GetPreConnectStatus() returns true after this session node starts a join then this session node is in a pre-connect state.
	 */
	bool GetPreConnectStatus(void);

	/**
	 * Enables or disables the pre-connect mechanism for session nodes that are joining a session. When this mechanism is enabled the joining session node will be able to exchange packets with the master session node before the node properly joins the session. The function PreConnectHasFinished() can be called to signal that the node wishes to complete a connect to the session. This function should only be used before a session node is used to join the session. The function IsStable() will not return true until the pre-connect is finished. See also SetDataRetention().<br>
	 * The intended for this mechanism is as follows:<br>
	 * 1) SetPreConnect(true) and attempts to join a session. All calls to GetSessionID() will return kReplicaNetUnknownUniqueID until after this session finishes the pre-connect and the master session transmits the correct sessionID.<br>
	 * 2) When GetPreConnectStatus() returns true after this session node starts a join then this session node is in a pre-connect state.<br>
	 * 3) This node will send one or more user defined packets that signal that the session wants to exchange certain pre-connect data to the master session using DataSendToSession() and session ID kReplicaNetUnknownUniqueID. Using the reliable packet type is recommended but not mandatory.<br>
	 * 4) The master session will receive user data packets and detecting that pre-connect data has been received will in reply send a user data message to the effect that it has received this information.<br>
	 * 5) The joining node receives pre-connect data from the master session, verifies the data and either completes the pre-connect by calling PreConnectHasFinished() or uses Disconnect() to abort the connection.<br>
	 * \param enable Set this to be true to enable the pre-connect mechanism for this session node. If false then the session node will discard all pre-connect packets and the pre-connection mechanism is assumed to be disabled. The default is false.
	 */
	void SetPreConnect(const bool enable = false);

	/**
	 * Allows the status of the pre-connect mechanism for this session node.
	 * \return Returns true if the pre-connect mechanism is enabled.
	 */
	bool GetPreConnect(void);

	/**
	 * If the pre-connect mechanism is enabled then this function must be called by the joining session node when packets have been exchanged to the satisfaction of the joining session node.
	 */
	void PreConnectHasFinished(void);

	/**
	 * Allows the raw data retention policy to be configured. By default ReplicaNet will discard raw data packets from the DataSend() or DataReceive() mechanism.
	 * \param enable Set this to be true to enable retention of raw data packets. The default value is false, which means ReplicaNet will discard any raw data packets.
	 */
	void SetDataRetention(const bool enable = false);

	/**
	 * Allows the raw data retention policy to be read.
	 * \return The data retention policy.
	 */
	bool GetDataRetention(void) const;

	/**
	 * Sends raw data from this session to any other session via it's ID or to all sessions by using kReplicaNetBroadcastID. If the node is in a pre-connect state and wants to send data to the master session then use kReplicaNetUnknownUniqueID as the session ID to send to.  See also SetDataRetention().<br>
	 * Data packets larger than 512 bytes can be sent and received correctly however these large packets may cause a performance hit as the large chunk of data is sent.
	 * A sessionID will not receive any data sent to itself either by using its own sessionID or from a broadcast.
	 * \param sessionid the session id to send to or kReplicaNetBroadcastID
	 * \param data the pointer to the data to send
	 * \param length the length of the data to send.
	 * \param type The type of data packet to send
	 * \param band The band to use for the underlying Transport when sending with the Reliable method.
	 */
	void DataSend(const int sessionid,const void *data,const int length,const PacketType type = kPacket_Reliable,const unsigned int band = 0);

	/**
	 * Receives raw data and responds with what session sent this data. See also SetDataRetention().
	 * \param fromsessionid The session id that sent this data.
	 * \param data The buffer for the data.
	 * \param length The length of the data received.
	 * \param type A pointer that can receive the packet type. This parameter can be null in which case the type is not filled in. The default value for this pointer is null.
	 * \return True if data was received
	 */
	bool DataReceive(int *const fromsessionid,void *const data,int *const length,PacketType *const type = 0);

	/**
	 * This allows the parameters of the next pending received data to be read. The data is left waiting to be read by DataReceive.
	 * \param fromsessionid The session id that sent this data.
	 * \param length The length of the data received.
	 * \param type A pointer that can receive the packet type. This parameter can be null in which case the type is not filled in. The default value for this pointer is null.
	 * \return True if data was received
	 */
	bool DataReceivePeek(int *const fromsessionid,int *const length,PacketType *const type = 0);
	

	/**
	 * This sets the update policy for all registered DataBlock classes for every ReplicaObject. By default ReplicaNet will automatically update the each DataBlock when the Poll() method is called or regularly when using threads. However by calling this method with false will disable the automatic updates and the user must then call ProcessDataBlockUpdate() to update each DataBlock.
	 * This is useful if the user wants to uncouple the DataBlock updates from the internal ReplicaNet operations.
	 * \param automatic Enable or disable automatic DataBlock updates. The default is true, which enables automatic updates.
	 */
	void SetDataBlockUpdatePolicy(const bool automatic = true);

	/**
	 * Returns the state set by SetDataBlockUpdatePolicy().
	 * \return the state set by SetDataBlockUpdatePolicy().
	 */
	bool GetDataBlockUpdatePolicy(void) const;

	/**
	 * If SetDataBlockUpdatePolicy(false) is used this function must be called by the user. See SetDataBlockUpdatePolicy() for more informaiton. If automatic updates are being used calling this function has no effect.
	 */
	void ProcessDataBlockUpdate(void);

	/**
	 * Enables or disables the automatic packet compression test. If enabled XPSession will test each packet sent to see if it can be compressed and if there is a space saving then the packet is compressed before sending.
	 * \param enable Enable or disable automatic packet compression. The default is false to disable automatic packet compression.
	 */
	void SetAutomaticPacketCompression(const bool enable = false);

	/**
	 * Gets the value configured by SetAutomaticPacketCompression()
	 * \return Returns true is the automatic packet compression is enabled.
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
	 * \param sent The total number of merged sent packets. This can be null.
	 * \param received The total number of merged sent packets. This can be null.
	 * \param reset If true, after returning the information to the caller both totals are reset back to zero. The default value is false.
	 */
	void GetMergedStatistics(int *const sent=0,int *const received=0,const bool reset = false);

	/**
	 * Gets the master session unique ID. If the session is not connected then the return value is kReplicaNetUnknownUniqueID.
	 * \return the master session unique ID
	 */
	int GetMasterSessionID(void);

	/**
	 * Attempts to resolve the URL of the session unique ID from the point of view of this session node.
	 * If the session unique ID is not known to the session then the result is a null string.
	 * If the session unique ID is not directly connected to this node, for example the spider connection was broken, then this function is not guaranteed to produce a URL and can return a null string.
	 */
	std::string GetURLFromSessionID(const int sessionID);

	/**
	 * Sets the client only flag for this session node. The default value is false which means this node will get joiner and leaver messages. If this is set to be true then this node will only get joiner/leaver messages for spider nodes or nodes that can become master nodes. This must not be changed after the session is used for joining a session.
	 * Setting this to option be true also automatically sets to be false SetCanBecomeMaster() and SetCanSpider()
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
	 * \param sessionID The session ID of the required node.
	 * \param bandwidth A pointer to accept the Transport::Bandwidth value.
	 * \param output A pointer to accept the output bandwidth value.
	 * \param input A pointer to accept the input bandwidth value.
	 * \return Success returns true, failure returns false.
	 */
	bool GetBandwidthFromSessionID(const int sessionID,Transport::Bandwidth *const bandwidth,int *const output,int *const input);

	/**
	 * This virtual callback method may be implemented to override any ROL registered ReplicaObject factory allocate function.
	 * If a ReplicaObject is returned then ReplicaObject::GetClassID() must be implemented in the derived class in such a way as to return the correct classID. The ROL utility functions usually perform this task for you.
	 * If the returned ReplicaObject instance was constructed before the ReplicaNet instance it will not be linked to the ReplicaNet instance, so ReplicaObject::GetNewSessionIDAndUniqueID() must be called after ReplicaNet is created and before or during this callback.
	 * \param classID The classID of the object to create.
	 * \param sessionID The sessionID of the object to create.
	 * \param uniqueID The uniqueID of the object to create.
	 * \return The default implementation is to return null which allows ReplicaNet to use the ROL registered factory.
	 */
	virtual ReplicaObject *CallBackAllocate(const int classID,const int sessionID,const int uniqueID);

	/**
	 * This virtual callback method may be implemented to override any ROL registered ReplicaObject factory delete function.
	 * \param object The classID of the object to delete.
	 * \return The default implementation is to return the input object pointer, this allows ReplicaNet to use the ROL registered factory on the returned object pointer. For example it would also be possible to return a different object pointer to that passed into the callback. Returning null will signal to ReplicaNet that the object has been handled.
	 */
	virtual ReplicaObject *CallBackDelete(ReplicaObject *object);

	/**
	 * Starts recording the network session. When the ReplicaNet session is created or joined recording of the entire session will begin.
	 * This function may be called at any time, for example before the session is created, joined or during the period when the session is running.
	 * CallbackSessionRecord() will then be called when ReplicaNet needs to save data. The user must implement storing this data.
	 * \return Success returns true, failure returns false.
	 */
	bool BeginSessionRecord(void);

	/**
	 * Stops recording the network session.
	 */
	void StopSessionRecord(void);

	/**
	 * This callback is called when ReplicaNet wants to save data from a recording session. The user must implement this function if recording is required.
	 * \param data The data to store.
	 * \param length The size in bytes of the data to store.
	 * \return Success returns true, failure returns false.
	 */
	virtual bool CallbackSessionRecord(const void *data,const int length);

	/**
	 * Allows the recording status of the session to be read.
	 * \return If true this means BeginSessionRecord() has been used and the session is currently recording.
	 */
	bool GetSessionRecord(void) const;

	/**
	 * Starts playback of a recorded network session.
	 * This function must be called before any ReplicaObject derived classes are published and before any network session is joined or created using this ReplicaNet instance.
	 * All of the session events are then simulated from the recording so a SessionJoin() or SessionCreate() is not required.
	 * CallbackSessionPlayback() will then be called when ReplicaNet needs to read in data. The user must implement reading this data.
	 * \return Success returns true, failure returns false.
	 */
	bool BeginSessionPlayback(void);

	/**
	 * Stops playback of a recorded network session.
	 */
	void StopSessionPlayback(void);

	/**
	 * Sets the speed of the playback.
	 * \param speed The speed for the playback. Smaller values produce slower playback and larger values produce faster playback. The default is 1.0.
	 */
	void SetPlaybackSpeed(const float speed = 1.0f);

	/**
	 * This advances the playback simulation to a specified network time.
	 * \param time The network time to advance to.
	 * \return True is returned if the simulation time has been advanced. False is returned if time is already in the past.
	 */
	bool PlaybackAdvanceTo(const SysTimeType time);

	/**
	 * This callback is called when ReplicaNet finishes advancing to the time set by PlaybackAdvanceTo().
	 */
	virtual void CallbackPlaybackAdvanceToFinished(void);

	/**
	 * This callback is called when ReplicaNet wants to get data from a session recording. The user must implement this function if playback is required.
	 * \param data The destination data pointer for the data to be stored.
	 * \param length The requested amount of data in bytes that ReplicaNet wants. If the user cannot supply the exact length of data then false should be returned.
	 * \return Success returns true, failure returns false.
	 */
	virtual bool CallbackSessionPlayback(void *data,const int length);

	/**
	 * Allows the playback status of the session to be read.
	 * \return If true this means BeginSessionPlayback() has been used and the session is currently playing back a recording.
	 */
	bool GetSessionPlayback(void) const;

	/**
	 * When playing back a recorded session the defines the if a recorded master ReplicaObject derived classes is kept
	 * as a master or is marked as a replica. For some applications it may be useful to always playback classes as replicas
	 * so that the application code does not try to control what was a master class.
	 * \param allow If this is true, what were recorded master classes will stay as masters. If this is false, what were master classes will be replicas when the recorded session is played back. The default is false.
	 */
	void SetPlaybackAllowMasterReplicaObjects(const bool allow = false);

	/**
	 * Returns the value configured by SetPlaybackAllowMasterReplicaObjects().
	 */
	bool GetPlaybackAllowMasterReplicaObjects(void);

	/**
	 * Enables or disables any further connection attempts to this session.
	 * \param allow Set this to be true to allow connections to this session. False will stop any further connection attempts to this session and the connecting session will get kReplicaNet_ETRANSPORT_CLOSED from GetSessionErrorReason().
	 */
	void SetAllowConnections(const bool allow = true);

	/**
	 * Returns the value configured by SetAllowConnections()
	 * \return the value.
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

	/**
	 * This callback is called when a ReplicaNet session changes masterID.
	 */
	virtual void CallbackMasterSessionIDChanged(const int previousSessionID);

	/**
	 * Enables or disables the CallbackGetSessionID() callback for the user to create their own sessionIDs.
	 */
	void SetSessionIDCallback(const bool enable = false);

	/**
	 * See SetSessionIDCallback(). This function is called from the thread context of XPSession.
	 * The user must guarantee that the calculated sessionID is greater than 0 and less than 32768 and is unique for the entire network session.
	 * \return The session ID.
	 */
	virtual int CallbackGetSessionID(void);

	/**
	 * Gets the next object unique ID to use from this ReplicaNet class. This can be overridden to allow unique IDs to be calculated by the user.
	 * The user must guarantee that the calculated unqiueID is unique for the local instance of ReplicaNet. The full 32 bits can be used.
	 * If virtual methods from the ReplicaObject pointer need to be called then use ReplicaObject::SetDelayIDFetch(true) to avoid the situation where this callback can be triggered during ReplicaObject construction.
	 * \return the next unique ID to use
	 */
	virtual int CallbackGetObjectUniqueID(ReplicaObject *object);

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

	int GetPacketCompressionLevel(void);
	int GetLargePacketCompressionLevel(void);

	/**
	 * Instead of using discrete data compression for each data packet sent using XPSession, use the Transport packet compression instead. This can result in higher compression ratios.
	 * This must be called before SessionCreate() or SessionJoin().
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
friend class ReplicaObject;		/* A ReplicaObject has direct access to ReplicaNet the class */
friend class ReplicaNetPrivate;
friend class ReplicaNetDLLRegister;
	/**
	 * Adds a ReplicaObject class to this ReplicaNet class. The object can be a master or replica.
	 * The object is added via this call when it is 1) Published or 2) Allocated as a replica.
	 * \param object the object to add
	 */
	void AddObject(ReplicaObject *const object);

	/**
	 * Removes a ReplicaObject class from this ReplicaNet class. The object can be a master or replica.
	 * This routine does not free an object it only removes the ReplicaNet reference to the object. If the object is a master then this calls also notifies other ReplicaNet session nodes that this object should be deleted from the session.
	 * The object is removed via this call when it is 1) Unpublished or 2) Deleted.
	 * \param object the object to remove
	 * \param sendDelete By default a delete is sent to all interested sessionIDs, however if this is false an unpublish packet is sent instead.
	 */
	void RemoveObject(ReplicaObject *const object,const bool sendDelete = true);
	
	/**
	 * Gets the current make master flag from this ReplicaNet class
	 * \return returns the current flag for the master creation state
	 */
	bool GetMakeMaster(void);

	/**
	 * When a replica object (not master) is being allocated it calls this to retrieve the unique ID passed from the real real machine that created this object
	 * \return the real unique ID
	 */
	int GetMakeUniqueID(void);

	/**
	 * When a replica object (not master) is being allocated it calls this to retrieve the session ID passed from the real real machine that created this object
	 * \return the real session ID
	 */
	int GetMakeSessionID(void);

	void LoadChangedForObject(ReplicaObject *object,float previousLoad,float newLoad);

	static ReplicaNet*	mCurrentReplicaNet;	/**< The current replicanet being processed */

	ReplicaNetPrivate*	mPrivateReplicaNet;

	void				AddItem(s_RO_RegistryBlock *block);

	/**
	 * Allocates a replica object from a class ID
	 * \param class_id The registered class ID
	 * \param session_id The session ID to set for this ReplicaObject
	 * \param uniqueid the unique ID to set for this ReplicaObject
	 */
	ReplicaObject* AllocateReplicaObject(const int class_id,const int sessionid,const int uniqueid,const int reflect = kReplicaNetUnknownUniqueID);

	void ReplicaObjectDataSend(const int sessionid,const void *data,const int length,const ReplicaNet::PacketType type = ReplicaNet::kPacket_Reliable,ReplicaObject *source = 0,const unsigned int band = 0);
};

/**
 * This internal class provides an interface that allows static forward library linkage or DLL linkage with an application to enable object IDs to be registered
 */
class ReplicaNetDLLRegister
{
public:
	ReplicaNetDLLRegister(){};
	virtual ~ReplicaNetDLLRegister(){};

	void Call(ReplicaNet *repNet);
};

} // namespace RNReplicaNet

extern void *_gReplicaNet_PreRegisterReplicasFunc;
// This function is created when parsing an ROL file containing ReplicaObject definitions. Or it can contain user generated code.
extern void _ReplicaNetPreRegisterReplicasFunc(RNReplicaNet::ReplicaNet *repNet);

#endif
