/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __REPLICAOBJECT_H__
#define __REPLICAOBJECT_H__

#include <vector>
#include <list>
#include <set>

#include "RNPlatform/Inc/ThreadClass.h"
#include "RNPlatform/Inc/SysTime.h"
#include "RNReplicaNet/Inc/DataBlock.h"

namespace RNReplicaNet
{

class ReplicaNet;
class ReplicaNetPrivate;
struct s_RO_RegistryBlock;

const float kReplicaObject_InfiniteDistance = -1.0f;	/**< The constant for an infinite distance between two objects. */
const int kReplicaObjectUnknownUniqueID = -1;

/**
 * A ReplicaObject is the base class that all replica objects use.<br>
 * A ReplicaObject can be created before or after a ReplicaNet instance. If a ReplicaObject is created before a ReplicaNet instance it is advisable, but not required, to call GetNewSessionIDAndUniqueID() with the ReplicaNet pointer when it is allocated.
 * However if a ReplicaObject derived class is not intended to be used in a network session there is no requirement to allocate an instance of ReplicaNet.
 * For example, if an application is started without ReplicaNet networking then instances of ReplicaObject will do nothing and use a nominal amount of memory.<br>
 * Each ReplicaObject class member variable propagated with a DataBlock can have a different packet update type (Unreliable, Certain or Reliable) to the ReplicaObject.
 * When a ReplicaObject is propagated to a session all class member variables that need to send an initial update will temporarily use the update type of the ReplicaObject.
 * This means a ReplicaObject propagated with the Certain update and with a class member variable normally propagated using Unreliable will have the initial variable propagated using Certain instead.
 * This is to guarantee the initial object state is propagated along with the object propagation packet.
 */
class ReplicaObject : private MutexClass
{
public:
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
	ReplicaObject();

	/**
	 * The dtor makes sure everything is tidy. If this object has been published the dtor also calls ReplicaNet::RemoveObject(this) to remove the references ReplicaNet holds about this object.
	 */
	virtual ~ReplicaObject();

	/**
	 * Tests the status of the object to see if it is a master object
	 * \return Returns true if this object is a replica (copy) from the network
	 */
	bool IsReplica(void) const;

	/**
	 * Tests the status of the object to see if it is a master object
	 * \return Returns true if this object is a master copy for the network
	 */
	bool IsMaster(void) const;

	/**
	 * Tests the status of the object to see if it is published, i.e. registered on the network session.
	 * \return Returns true if this object is published.
	 */
	bool IsPublished(void) const;

	/**
	 * Publish this object on to the network. This uses ReplicaNet::AddObject(this) to add a reference of this object to ReplicaNet.
	 * If the object has already been registered with ReplicaNet (i.e. it is a replica or has already been published) this will generate an assert in debug code and have no effect in release code.
	 */
	void Publish(void);

	/**
	 * Unpublish this object from the network. If the object is a master then this will unpublish the object on all replicas. If the object is a replica then this will only unpublish the object on this node.
	 * This object will not send network updates using ReplicaNet.
	 * The user callback function CallbackObjectUnpublishPre() and CallbackObjectUnpublishPost() will be called and the object will unregister itself from all ReplicaNet object lists but will remain allocated for the user to handle.
	 */
	void Unpublish(void);

	/**
	 * Gets the class id that is set
	 * \return the class id for this class
	 */
	virtual const int GetClassID(void) = 0;

	/**
	 * Sets the registered flag to the bool value
	 * \param flag the value to set the flag to, true is default
	 */
	virtual void SetRegistered(bool flag = true) = 0;

	/**
	 * Returns the registered flag status of this class
	 * \return the registered state of this class
	 */
	virtual bool GetRegistered(void) = 0;

	/**
	 * Returns the unique id of the object local to the session. To identify an object for the entire network session you need to use GetUniqueID() and GetSessionID().
	 * \return the unique id
	 */
	int GetUniqueID(void);

	/**
	 * Returns the session id of the object which is global for the whole network session
	 * \return the session id
	 */
	int GetSessionID(void);

	/**
	 * Gets the object's bound ReplicaNet class
	 * \return the ReplicaNet pointer that is bound to this object
	 */
	ReplicaNet *GetBoundReplicaNet(void);

	/**
	 * Use this virtual callback to approve or deny that this object is fault tolerant and can change owner when a session leaves.
	 * \return To approve return true else return false. The default is false
	 */
	virtual bool ApproveFault(void);

	/**
	 * Use this virtual callback to approve or deny that this object can migrate when a migrate request is made.
	 * \return To approve return true else return false. The default is false
	 */
	virtual bool ApproveMigration(void);

	/**
	 * This callback is called by ReplicaNet whenever an object changes owner.
	 */
	virtual void OwnerChange(void);

	/**
	 * This tries to change the ownership on a ReplicaObject to another session.
	 * This only succeeds if the ReplicaObject is a Master object.
	 * This only succeeds if the ApproveMigration() callback returns true for the object.
	 * \param sessionID the target sessionID for this object to migrate to
	 */
	void GiveOwnership(int sessionID);

	/**
	 * This requests a change of the ownership on a ReplicaObject to be the calling session.
	 * This only succeeds if the ReplicaObject is a Replica.
	 * This only succeeds if the ApproveMigration() callback returns true for the object.
	 */
	void RequestOwnership(void);

	/**
	 * The callback for calculating the distance to an object. This can (and should) be replaced by the application's object types. Optimisation note, using ReplicaNet registered observers the target object will usually be a registered observer.
	 * \param object The destination object to use, if this parameter is null then kReplicaObject_InfiniteDistance should be returned.
	 * \return the distance to the object. The default return value is kReplicaObject_InfiniteDistance meaning that the distance between objects could not be calculated.
	 */
	virtual float CalculateDistanceToObject(ReplicaObject *object);

	/**
	 * Gets the distance to another supplied ReplicaObject. This function applies a caching mechanism to avoid too many unwanted calculations. Optimisation note, using ReplicaNet registered observers the target object will usually be a registered observer.
	 * \param object The destination object to use. If the object pointer is null then kReplicaObject_InfiniteDistance is returned.
	 * \return the distance to the object or kReplicaObject_InfiniteDistance meaning the distance between two objects couldn't be calculated.
	 */
	float GetDistanceToObject(ReplicaObject *object);

	/**
	 * This is a useful virtual call that can be used to set an opaque pointer to classes or data in a ReplicaObject or the derived class
	 * \param data the pointer to the opaque data
	 */
	virtual void SetOpaquePointer(void *data);

	/**
	 * This is a useful virtual call that can be used to obtain opaque pointers to classes or data in a ReplicaObject or the derived class
	 * \param data the pointer to some optional data that can be passed in. This defaults to null
	 * \return the opaque data pointer
	 */
	virtual void *GetOpaquePointer(void *data = 0);

	/**
	 * Given a pointer to memory this functions finds a data block attached to that memory area
	 * \param pdata the pointer to the memory area
	 * \return the DataBlock pointer or NULL if no data block is attached
	 */
	DataBlock *FindDataBlock(void *pdata);

	/**
	 * Returns a DataBlock derived class attached to the class member.
	 */
	template<typename dataBlockType , typename dataType>
	dataBlockType *FindDataBlock(dataType &member)
	{
		DataBlock *datablock = FindDataBlock(&member);
		if (!datablock)
		{
			return 0;
		}
		if (datablock->IsClassType(dataBlockType::StaticThisClassType()))
		{
			dataBlockType *realBlock = (dataBlockType*)(datablock);
			return realBlock;
		}
		return 0;
	}

	/**
	 * This gives a delta hint for a particular variable that has a float type. The delta should be expressed in units of change per second.
	 * \param variable the variable to give a hint for
	 * \param delta the delta hint
	 */
	void GiveDeltaHint(float &variable,float delta);

	/**
	 * This is used to set a continuity break for the data set and provides useful information to the extrapolation algorithm
	 * \param variable the variable to give a hint for
	 * \param breakTypes The break types to use for this break. For example : DataBlock::kSuddenChange | DataBlock::kTeleport
	 */
	void ContinuityBreak(float &variable,unsigned char breakTypes);

	/**
	 * This sets this object to use the 'reliable' packet type when transmitting information about this object to other sessions.<br>
	 * By default the object uses the reliable method.<br>
	 * Each datablock then uses the type of update assigned to itself.
	 */
	void UpdateSetReliable(void);

	/**
	 * This sets this object to use the 'certain' packet type when transmitting information about this object to other sessions.<br>
	 * By default the object uses the reliable method.<br>
	 * Each datablock then uses the type of update assigned to itself.
	 */
	void UpdateSetCertain(void);

	/**
	 * This sets this object to use the 'normal' packet type when transmitting information about this object to other sessions.<br>
	 * By default the object uses the reliable method.<br>
	 * Each datablock then uses the type of update assigned to itself.
	 */
	void UpdateSetUnreliable(void);

	/**
	 * This sets this object to use the 'normal ordered' packet type when transmitting information about this object to other sessions.<br>
	 * By default the object uses the reliable method.<br>
	 * Each datablock then uses the type of update assigned to itself.
	 */
	void UpdateSetOrdered(void);

	/**
	 * Tests the update type for this object.
	 * \return true if this object uses the 'reliable' packet type.
	 */
	bool UpdateIsReliable(void);

	/**
	 * Tests the update type for this object.
	 * \return true if this object uses the 'certain' packet type.
	 */
	bool UpdateIsCertain(void);

	/**
	 * Tests the update type for this object.
	 * \return true if this object uses the 'certain' packet type.
	 */
	bool UpdateIsUnreliable(void);

	/**
	 * Tests the update type for this object.
	 * \return true if this object uses the 'certain' packet type.
	 */
	bool UpdateIsOrdered(void);

	/**
	 * Enables or disables load balancing for this ReplicaObject. For this object to migrate due to load balancing ReplicaObject::ApproveMigration() must return true, the corresponding ROL script command can also be used.
	 * \param enable set this to true to enable load balancing. The default is false
	 */
	void SetLoadBalancing(const bool enable = false);

	/**
	 * This sets the default value that the load balancing uses. Setting the default value in the object constructor to the same value that is set for the load balancing will result in a smaller data packet being sent.
	 * \param defaultValue the value that is the default value. The default is false to match the parameter to SetLoadBalancing()
	 */
	void SetLoadBalancingDefault(const bool defaultValue = false);

	/**
	 * Gets the status of the load balancing flag
	 * \return true if load balancing is enabled
	 */
	bool GetLoadBalancing(void);

	/**
	 * Sets the object load unit score..
	 * The definition of "load" units is left to the user. The default return value is 0.0f
	 * \param load A "load" score for this object.
	 */
	void SetLoadScore(const float load = 0.0f);

	/**
	 * This sets the default value that the load balancing score uses. Setting the default value in the object constructor to the same value that is set for the load balancing will result in a smaller data packet being sent when an object is published.
	 * When setting a default value the same value must be set on the master and all replica objects otherwise the different sessions will assume different default values. If a replica is unable to know that other replicas will set the same default value then not using the function will ensure that the correct value is propagated.
	 * \param defaultLoad the value that is the default value. The default is 0.0f to match the parameter to SetLoadScore()
	 */
	void SetLoadScoreDefault(const float defaultLoad = 0.0f);

	/**
	 * Gets the object to report a score based on its "load" for the master session.
	 * The definition of "load" units is left to the user. The default return value is 0.0f
	 * \return A "load" score for this object.
	 */
	float GetLoadScore(void);

	/**
	 * This virtual method callback is called by ReplicaNet when a class is created and the initial data from the data blocks has been filled in. The user can choose to override this function if needed. The base class method does not need to be called.
	 */
	virtual void PostObjectCreate(void);

	/**
	 * Allows the user to expand the datablock mechanism with custom registering of datablocks if needed. It is required that the same DataBlocks are registered in the same order for all master and replica objects.
	 * These datablocks are registered after the internal ReplicaObject, ROL derived class datablocks and auto-added datablocks. Any datablocks added by this function should be added using RegisterDataBlock() and not AddAutoRegisterDataBlock()
	 * Do not call this function directly, this function is only called by ReplicaNet.
	 */
	virtual void UserRegisterDataBlocks(void);

	/**
	 * This function allows DataBlocks to automatically register when a ReplicaObject is created.  It is required that the same DataBlocks are registered in the same order for all master and replica objects.
	 * \param dataBlock the pointer to the DataBlock to register
	 */
	void AddAutoRegisterDataBlock(DataBlock *const dataBlock);

	/**
	 * This function allows an object to state which sessionID should be used for sending messages to other sessions.
	 * By default the master object is used to send DataBlock and message updates.
	 * \param sessionID the session to be used for sending message updates, the default is kReplicaObjectUnknownUniqueID which signifies that the master sends messages. If the reflection ID is the same as the master session ID then the reflection ID change is ignored.
	 */
	void SetMessageReflectionID(const int sessionID = kReplicaObjectUnknownUniqueID);

	/**
	 * This function gets the sessionID used to reflect message updates
	 * \return The sessionID that isused to reflect messages, kReplicaObjectUnknownUniqueID means that the master object sends message updates
	 */
	int GetMessageReflectionID(void) const;

	/**
	 * This function sets the distance that an object has to be within for it to be propagated relative to a session observer.
	 * If a session has an observer and the CalculateDistanceToObject() result is greater than the value specified then the session will not receive object updates.
	 * If a session has no registered observer then the object will always be propagated to that session. It is therefore a good idea to publish and set the observer for a session before joining a ReplicaNet session so that the joining session does not receive objects that will then quickly get removed again.
	 * The default value for a ReplicaObject is to have this distance set to kReplicaObject_InfiniteDistance, which means that the object will always be propagated to a session.
	 * This function can only be used if the ReplicaObject is a master and SetSessionPropagationFilter() is not activated. This distance can be changed at runtime.
	 * To ensure this functionality is enabled the first use of this function must be before the object is published using the Publish() method.
	 * If the ReplicaObject owner changes, notified through the OwnerChange() callback, then SetPropagateToSessionDistance() must be called to enable this functionality on the new master.
	 * \param distance the distance to set, kReplicaObject_InfiniteDistance is the default value
	 */
	void SetPropagateToSessionDistance(const float distance = kReplicaObject_InfiniteDistance);

	/**
	 * Similar to SetPropagateToSessionDistance() this function sets the minimum distance where objects will be included in the distance based propagation.
	 */
	void SetPropagateToSessionDistanceInclude(const float distance = kReplicaObject_InfiniteDistance);

	/**
	 * Similar to SetPropagateToSessionDistance() this function sets the maximum distance where objects will be excluded from the distance based propagation.
	 */
	void SetPropagateToSessionDistanceExclude(const float distance = kReplicaObject_InfiniteDistance);

	/**
	 * This sets the default value that the propagate distance uses. Setting the default value in the object constructor to the same value that is set for the distance will result in a smaller data packet being sent when an object is published.
	 * When setting a default value the same value must be set on the master and all replica objects otherwise the different sessions will assume different default values. If a replica is unable to know that other replicas will set the same default value then not using the function will ensure that the correct value is propagated.
	 * \param distance the value that is the default value. The default is kReplicaObject_InfiniteDistance to match the parameter to SetPropagateToSessionDistance()
	 */
	void SetPropagateToSessionDistanceDefault(const float distance = kReplicaObject_InfiniteDistance);

	/**
	 * Similar to SetPropagateToSessionDistanceDefault() this function sets the default value for SetPropagateToSessionDistanceInclude().
	 */
	void SetPropagateToSessionDistanceIncludeDefault(const float distance = kReplicaObject_InfiniteDistance);

	/**
	 * Similar to SetPropagateToSessionDistanceDefault() this function sets the default value for SetPropagateToSessionDistanceExclude().
	 */
	void SetPropagateToSessionDistanceExcludeDefault(const float distance = kReplicaObject_InfiniteDistance);

	/**
	 * Allows the distance value to be read for this ReplicaObject set by SetPropagateToSessionDistance().
	 * This function can be used on a master or replica ReplicaObject
	 * \return the distance value
	 */
	float GetPropagateToSessionDistance(void) const;

	/**
	 * Similar to GetPropagateToSessionDistance() returns the value set by SetPropagateToSessionDistanceInclude().
	 */
	float GetPropagateToSessionDistanceInclude(void) const;

	/**
	 * Similar to GetPropagateToSessionDistance() returns the value set by SetPropagateToSessionDistanceExclude().
	 */
	float GetPropagateToSessionDistanceExclude(void) const;

	/**
	 * This function sets the minimum recalculation delay that this object will use when it calculates distance based propagation.<br>
	 * If the object owner changes the delay time will revert back to the default value. The object will, if enabled, automatically recalculate the list of sessions to propagate to.
	 * \param delay The recalculation delay in seconds, the default is 0.1 seconds. Values less than 0 are ignored.
	 */
	void SetPropagateToSessionDistanceRecalculationDelay(const float delay = 0.1f);

	/**
	 * This static function can be defined in a derived class to enable the allocation of a ReplicaObjet derived class to be implemented. This allocation must be implemented in such a way that the ReplicaObject constructor is called when this function is called. The default implementation is to use new.
	 * \return A pointer to an object that is derived from ReplicaObject.
	 */
	static ReplicaObject *AllocateForReplicaNet(void);

	/**
	 * This static function can be defined in a derived class to enable the de-allocation of a ReplicaObjet derived class to be implemented. This allocation must be implemented in such a way that the ReplicaObject destructor is called when this function is called. The default implementation is to use delete.
	 * \param object A pointer to an object that is derived from ReplicaObject.
	 */
	static void DeleteForReplicaNet(ReplicaObject *object);


	/**
	 * This allows the session propagation filter mechanism to be enabled or disabled for a master ReplicaObject.
	 * When enabled the ReplicaObject will ignore all joining sessions and the user application is then responsible for supplying the session IDs that this object will propagate to. This mechanism effectively turns off all ReplicaNet object propagation functions for this object, including the distance based propagation. This mechanism can only be enabled or disabled before the object is published to ReplicaNet.
	 * If the object is set to reflect updates, using SetMessageReflectionID(), then the reflect session ID must also be added to the session propagation list.
	 * If the ReplicaObject owner changes, notified through the OwnerChange() callback, then the previous list of propagated sessions is discarded. SetSessionPropagationFilter() must be called to enable this functionality on the new master and the sessions to propagate to must again be added using SessionPropagationAddSession().
	 * \param enable When true enables this mechanism. The default value is false and this mechanism is disabled.
	 */
	void SetSessionPropagationFilter(const bool enable = false);

	/**
	 * Allows the status of the session propagation filter mechanism to be read.
	 * \param Returns true if the mechanism is enabled
	 */
	bool GetSessionPropagationFilter(void) const;

	/**
	 * Gets the current propagation set.
	 * \param theSet The current set is stored in this reference.
	 */
	void GetSessionPropagationSet(std::set<int> &theSet);

	/**
	 * When the session propagation filter mechanism is enabled this function tells this ReplicaObject to propagate to the supplied session ID. This function will only operate correctly when the session propagation filter mechanism is enabled. The session ID must be a valid ID known to the network session. The session ID from the callback ReplicaNet::JoinerSessionIDPre() can be safely added to objects. If the session ID has already been added to the list (without a corresponding remove) then ReplicaNet will ignore extra additions.
	 * \param sessionID The session ID to propagate to.
	 * \return Returns true if the added sessionID was not in the list. Returns false if the sessionID was in the propagation list.
	 */
	bool SessionPropagationAddSession(const int sessionID);

	/**
	 * When the session propagation filter mechanism is enabled this function tells this ReplicaObject to finish propagating to the supplied session ID. This function will only operate correctly when the session propagation filter mechanism is enabled. The session ID must be a valid ID known to the network session.  The session ID from the callback ReplicaNet::LeaverSessionIDPre() can be safely added to objects. If the session ID has already been removed from the list (without a corresponding add) then ReplicaNet will ignore extra remove requests.
	 * \param sessionID The session ID to finish propagating to.
	 * \return Returns true if the removed sessionID was in the list. Returns false if the sessionID was not in the propagation list.
	 */
	bool SessionPropagationRemoveSession(const int sessionID);

	/**
	 * This virtual callback is triggered by ReplicaNet every time an update packet for a DataBlock is received, but before the packet data is processed. See also FindDataBlock()
	 * \param datablock The pointer to the DataBlock that is getting the update.
	 */
	virtual void DataBlockPacketDataReceived(const DataBlock *datablock);

	/**
	 * This virtual callback is triggered by ReplicaNet every time an update packet for a DataBlock is received, after the packet data is processed. See also FindDataBlock()
	 * \param datablock The pointer to the DataBlock that has received the update.
	 */
	virtual void DataBlockPacketDataReceivedPost(const DataBlock *datablock);

	/**
	 * A pure virtual access method to return the registry block associated with this ReplicaObject class type. This registry block is set when the ReplicaObject derived class is registered with ReplicaNet and the standard interface is implemented by the macro _RO_DO_STD_FRAMEWORK()<br>
	 * For example, to get the registered class name of an unknown "ReplicaObject *theObject" pointer, use the preprocessor to define REPLICANET_VISUALDEBUGGER and then use: theObject->GetRegistryBlock()->GetName().
	 * \return the pointer to the s_RO_RegistryBlock. The registry block contents must not be changed by the user. This function must always return a valid registry block pointer.
	 */
	virtual s_RO_RegistryBlock *GetRegistryBlock(void) const = 0;

	/**
	 * Sends raw data from this session to any other session via it's ID or to all sessions by using kReplicaNetBroadcastID.<br>
	 * \param sessionid the session id to send to or kReplicaNetBroadcastID
	 * \param data the pointer to the data to send
	 * \param length the length of the data to send. Data packets larger than 512 bytes may be fragmented and sent in separate chunks.
	 * \param type The type of data packet to send
	 * \param band The band to use for the underlying Transport when sending with the Reliable method.
	 */
	void DataSend(const int sessionid,const void *data,const int length,const PacketType type = kPacket_Reliable,const unsigned int band = 0);

	/**
	 * Use this virtual callback to process raw data sent by DataSend().
	 * Processing for this function may occur during the thread context of ReplicaNet so processing time used by this function should be kept to a minimum to avoid performance issues.
	 * \param fromsessionid The session id that sent this data.
	 * \param data The buffer for the data.
	 * \param length The length of the data received.
	 * \param type The packet delivery type.
	 */
	virtual void CallBackDataReceive(const int fromsessionid,const void *data,const int length,const PacketType type);

	/**
	 * When processing DataBlock updates this will return the sessionID of the originator of those updates.
	 * Usually this will be the master sessionID of the object however when using a reflecting object or a master callback this function will return these sessionIDs. If no DataBlock is being processed then kReplicaNetUnknownUniqueID will be returned.
	 * \return A sessionID.
	 */
	int GetProcessingDataBlocksFromSessionID(void);

	/**
	 * Use this virtual callback to detect when this object unlinks itself from the ReplicaNet object lists due to ReplicaObject::Unpublish() being used.
	 * This is called just before the unlinking procedure. The sessionID and uniqueID values are still valid at this point.
	 */
	virtual void CallbackObjectUnpublishPre(void);

	/**
	 * Use this virtual callback to detect when this object unlinks itself from the ReplicaNet object lists due to ReplicaObject::Unpublish() being used.
	 * This is called just after the unlinking procedure. The old sessionID and uniqueID values are not valid at this point however the object can be published again if required and it will behave as a new master object from this node.
	 */
	virtual void CallbackObjectUnpublishPost(void);

	/**
	 * Care must be taken when using this function as it forces a new sessionID and uniqueID to be fetched from ReplicaNet. See SetDelayIDFetch().
	 * \param toBindTo The ReplicaNet instance this object should be bound to. The default value is null and in this case will use the ReplicaNet instance currently bound to this ReplicaObject instance during the constructor.
	 * \return true indicates the operation succeeded. False indicates the operation failed mostly likely due to there being no ReplicaNet instance or the object already being registered with ReplicaNet.
	 */
	bool GetNewSessionIDAndUniqueID(RNReplicaNet::ReplicaNet *toBindTo = 0);

	/**
	 * This will flag this ReplicaObject for a delete by ReplicaNet when delay amount of time elapses, the default is as soon as possible, but will delay the delete until any object DataBlock update finishes. This is especially useful during a network function call since objects must not be deleted during these function calls.
	 * \param delaySeconds The amount of time to wait for the delete. The default is 0.0 seconds, meaning the delete will happen as soon as possible.
	 */
	void DelayedDelete(const float delaySeconds = 0.0f);

	/**
	 * Cancels any pending delayed delete set by DelayDelete().
	 */
	void DelayedDeleteCancel(void);

	/**
	 * Returns true if a delay delete is pending.
	 */
	bool GetDelayedDelete(void);

	/**
	 * Returns true if a delay delete is pending. The time is also returned in delaySeconds. If there is no delayed delete then delaySeconds will not be changed.
	 */
	bool GetDelayedDelete(float &delaySeconds);

	/**
	 * Allows the dormant state of this object to be set. Dormant objects use less processor time because DataBlocks are not checked. If true then this object will not send any DataBlock updates.
	 */
	void UpdateSendDormant(const bool dormant = false);

	/**
	 * Returns the dormant state of this object set by UpdateSendDormant() or by UpdateSendDormantDelay().
	 */
	bool GetUpdateSendDormant(void) const;

	/**
	 * Allows the dormant state of this object to be set when delaySeconds has elapsed. Dormant objects use less processor time because DataBlocks are not checked. If true then this object will not send any DataBlock updates. The default value for delaySeconds is 0.0f seconds and with this value this function is exactly the same operation as UpdateSendDormant().
	 * When delaySeconds has elapsed then UpdateSendDormant() will automatically be called with the configured dormant state.
	 */
	void UpdateSendDormantDelay(const bool dormant = false,const float delaySeconds = 0.0f);

	/**
	 * Cancels any request configured by UpdateSendDormantDelay()
	 */
	void UpdateSendDormantDelayCancel(void);

	/**
	 * Returns the configuration of this object set by UpdateSendDormantDelay().
	 * \return If there is no pending delayed dormant set then false is returned and the two input variables are left unchanged.
	 */
	bool GetUpdateSendDormantDelay(bool &dormant,float &delaySeconds) const;

	/**
	 * When the UpdateSendDormantDelay() delaySeconds has elapsed this callback will be called with the new state in dormant. The current dormant state of the object can be read by using GetUpdateSendDormant().
	 */
	virtual void UpdateSendDormantDelayStateChange(const bool dormant);

	/**
	 * Sets the band used by the underlying Transport when using the Reliable method to send this ReplicaObject update.
	 * The default value is 0 which is the same band as the internal session messages.
	 */
	void SetBand(const unsigned int band = 0);

	/**
	 * This function returns the value set by SetBand()
	 * \return The value.
	 */
	unsigned int GetBand(void) const;

	/**
	 * When true this causes the function GetNewSessionIDAndUniqueID() to be called after the constructor, so during Publish() or during ReplicaNet::AllocateReplicaObject().
	 * When true this allows Replicanet::CallbackGetObjectUniqueID(ReplicaObject *object) to call virtual methods from the ReplicaObject pointer because the constructor will have finished. This also means the object ID values will not be valid during the constructor and will only be valid after GetNewSessionIDAndUniqueID() has been called.
	 * The default value is false, this means GetNewSessionIDAndUniqueID() and hence ReplicaNet::CallbackGetObjectUniqueID() will be called during construction and means the object ID values will be valid when the derived class constructor is called. This does mean the object pointer during CallbackGetObjectUniqueID() cannot be used to call virtual methods because the object is being constructed.
	 */
	static void SetDelayIDFetch(const bool delay = false);

	/**
	 * Returns the value set by SetDelayIDFetch()
	 */
	static bool GetDelayIDFetch(void);

	/**
	 * When true this causes ReplicaObject instances to not bind to the currently available ReplicaNet instance during ReplicaObject construction.
	 * This will also cause the object to delay getting a session and unique ID during construction.
	 * Instead a specific call to GetNewSessionIDAndUniqueID() is needed to set the bound ReplicaNet instance.
	 * The default value is false, this means ReplicaNet::GetCurrentReplicaNet() is used during construction for the bound ReplicaNet instance.
	 */
	static void SetDelayBind(const bool delay = false);

	/**
	 * Returns the value set by SetDelayIDFetch()
	 */
	static bool GetDelayBind(void);

protected:
	// Default is to do nothing as it is set in the ctor
	// Otherwise it is used to set an update type by using a macroed function call specified in the ROLCompiler commands
	virtual void _Internal_GetUpdateType(void) {}
	virtual void _Internal_SetupLoadBalance(void) {}
	virtual void _Internal_SetupLoadBalanceScore(void) {}
	virtual void _Internal_SetupLoadBalanceDefault(void) {}
	virtual void _Internal_SetupLoadBalanceScoreDefault(void) {}
	virtual void _Internal_SetupOpaquePointer(void) {}
	virtual void _Internal_SetupPropagateToSessionDistanceRecalculationDelay(void) {}
	virtual void _Internal_SetupPropagateDistance(void) {}
	virtual void _Internal_SetupPropagateDistanceDefault(void) {}
	virtual void _Internal_SetupPropagateDistanceInclude(void) {}
	virtual void _Internal_SetupPropagateDistanceIncludeDefault(void) {}
	virtual void _Internal_SetupPropagateDistanceExclude(void) {}
	virtual void _Internal_SetupPropagateDistanceExcludeDefault(void) {}
	virtual void _Internal_SetupBand(void) {}

	// This sets the internal debugging full name
	void _Internal_SetFullyQualifiedName(const char *name = 0);
	const char *_Internal_GetFullyQualifiedName(void);
	// Debugging emit functions
	void _DebugInternal_EmitClassCreate(const int packetSize,const int toSession);
	void _DebugInternal_EmitClassDelete(const int packetSize,const int toSession);
	void _DebugInternal_EmitClassUnpublish(const int packetSize,const int toSession);

	/**
	 * Registers a datablock from the derived class to this class. This calls DataBlock::BindReplicaNet(ReplicaObject::GetBoundReplicaNet()), DataBlock::BindReplicaObject(ReplicaObject::this) and DataBlock::SetID(theNextInternalID)
	 * \param The data block to register
	 */
	virtual void RegisterDataBlock(DataBlock *block);

	virtual void SetNumExpectedDataBlocks(const int expected);

	/**
	 * Override this to allow the user to return how many DataBlock classes are expected to be registered with the derived ReplicaObject class and reserve memory for the internal storage.
	 * Alternatively call SomeReplicaObjectDerivedClass::StaticSetExpectedNumDataBlocks() to set the expected number of DataBlock classes for this object. For example this static method could be called from the derived object constructor.
	 * The expected number of DataBlock classes can be unique for each ReplicaObject derived class. Storage for this is initialised by the standard ROL setup macro _RO_DEF_SETUP_TYPE_SPECIFICS.
	 */
	virtual int GetNumExpectedDataBlocks(void);

private:
friend class ReplicaNet;			/* ReplicaNet has direct access to ReplicaObject classes */
friend class ReplicaNetPrivate;		/* ReplicaNet has direct access to ReplicaObject classes */
friend struct SessionUnique;		/* ReplicaNet has direct access to ReplicaObject classes */
friend class DataBlock;

// Functions
	// Safe to call multiple times
	virtual void PreRegisterDataBlocks(void);	/**< Pre-registers the data blocks for this ReplicaObject then calls RegisterDataBlocks() */
	// Do not call, except from within PreRegisterDataBlocks()
	virtual void RegisterDataBlocks(void);	/**< Registers the data blocks for this ReplicaObject, virtual because the derived class will want to override the default null behavior of this to register blocks */

	void PollInternalForDistance(const SysTimeType localTime,ReplicaObject *observer,const int sessionID);


// Variables
	bool mIsMaster;		/**< Flag to say if this class is a master or not */
	int mUniqueID;		/**< Unique ID that is used to reference copies of this object across sessions */
	int mSessionID;		/**< Unique session ID */
	ReplicaNet *mBoundReplicaNet;	/**< The ReplicaNet that is bound to this object */
	bool mBeingDeleted;
protected:
	bool mDataBlocksRegistered;
private:
	bool mFlagForLaterMigration;	/**< This flag is true if the object is flagged for migration when a new master session is chosen */
	void *mOpaquePointer;
	bool mOpaquePointerSet;
	bool mIsReliable;
	bool mIsCertain;
	bool mIsOrdered;
	int mTryingToGiveTo;
	bool mObjectHasBeenAdded;	/**< true when the object has been added to ReplicaNet, either when published or when allocated by ReplicaNet itself. */
	bool mEnableLoadBalancing;
	float mLoadScore;
	bool mEnableLoadBalancingDefault;
	float mLoadScoreDefault;
	int mReflectSessionID;		/**< If not kReplicaNetUnknownUniqueID then this tells ReplicaNet what sessionID is to be used for message refelction */
	float mDistanceToSessionPropagate;
	float mDistanceToSessionPropagateDefault;
	float mDistanceToSessionPropagateOuter;
	float mDistanceToSessionPropagateOuterDefault;
	bool mSessionPropagationFilter;

	std::vector<DataBlock *> mAutoRegister;
	// std::vector is used for mDataBlocks to allow fast constant time lookups by index.
	std::vector<DataBlock *> mDataBlocks;	/**< The data blocks registered to the derived class */
	std::set<int> mSessionPropagationFilterIDs;
	// These delay slots are for those pending create class or delete class messages that have not been sent yet.
	std::set<int> mSessionPropagationFilterDelaySlotAdd;
	std::set<int> mSessionPropagationFilterDelaySlotRemove;

	// Distance based optimisations
	bool mEnableDistanceBased;
	SysTimeType mDistanceBasedPollTime;
	SysTimeType mDistanceBasedLastTime;

	bool mMasterOrReplicaDataBlockPollDone;

	char *mFullyQualifiedName;
	bool mPostObjectCreateDone;
	
	std::set<int> mObserverFor;		// The session IDs that are using this object as an observer

	int mProcessingDataBlocksFromSessionID;
	bool mDelayDelete;
	float mDelayDeleteSeconds;
	SysTimeType mDelayDeleteTime;	// The local time to do the delete
	bool mUpdateSendDormant;

	bool mDormantDelayActive;
	bool mDormantDelayState;
	float mDormantDelaySeconds;
	SysTimeType mDormantDelayTime;

	unsigned int mBand;

	bool mGetNewSessionIDAndUniqueIDCalled;

	static bool mDelayIDFetch;
	static bool mDelayBind;
};

} // namespace RNReplicaNet

#endif
