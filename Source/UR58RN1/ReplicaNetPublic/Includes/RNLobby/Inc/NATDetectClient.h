/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _NATDETECTCLIENT_H_
#define _NATDETECTCLIENT_H_
#include "RNLobby/Inc/NATDetectServer.h"
#include "RNXPURL/Inc/Transport.h"

namespace RNReplicaNet
{

namespace RNLobby
{

/// This class interrogates the network connection of a machine to determine the if there is any NAT implemented to
/// known external machines.
class NATDetectClient : public BaseServer
{
public:
	enum NATType
	{
		kOpen = 0,
		kModerate,
		kStrict
	};

	NATDetectClient();
	virtual ~NATDetectClient();

	/// If using a specific game channel for ReplicaNet/XPSession then remember to stop the NATDetectClient service
	/// before calling ReplicaNet::SessionCreate()/XPSession::Create() so the game channel is free for ReplicaNet/XPSession to use.
	bool Start(const int channel = TransportAnyGameChannel);

	/// Sets the address and port of the servers. The default for the instance of the class is localhost and port NATDetectServer::kDefaultPort and NATDetectServer::kDefaultPort+1 or the address resolved by a successful BackendClient::Start().
	void SetServers(const XPAddress &address1,const XPAddress &address2);

	/// Tests the completion state of the last query. When the query completes the result can be retrieved by using GetReply()
	/// \return A completed query returns true, a pending query returns false.
	bool GetCompleted(void);

	/// Returns the last reply from a completed query.
	/// \return The last reply from a completed query.
	NATDetectServer::Reply GetReply(void);

	/// If GetReply() == NATDetectServer::kReply_Success this returns the type of NAT calculated by this class.
	NATType GetNATType(void) const;

	/// Returns the public internet address of this computer as seen by the NATDetectServer.
	XPAddress GetPublicAddress(void) const;

	/// A virtual callback that is triggered when the NATDetect calculation is complete. GetReply(), GetNATType() and GetPublicAddress() can be used during this callback or at any time after completion.
	virtual void CallbackCompleted(void) {}

	/// Gets the two addresses as seen by the NATDetectServer
	void GetAddressReplies(XPAddress &addr1,XPAddress &addr2);

private:
	void Init(void);
	bool CallbackParseValidMessage(DynamicMessageHelper &message,const size_t length,const XPAddress &address);
	bool DoSuccessState(void);

	bool CallbackPoll(void);

	SysTimeType mLastSentTime;
	int mGot1Times;
	int mGot2Times;
	int mTries;
	volatile NATDetectServer::Reply mReply;
	XPAddress mAddress1;
	XPAddress mAddress2;

	XPAddress mAddress1Reply;
	XPAddress mAddress2Reply;

	bool mIsNiceNAT;

	bool mGotOther;
	bool mServerAddressSet;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
