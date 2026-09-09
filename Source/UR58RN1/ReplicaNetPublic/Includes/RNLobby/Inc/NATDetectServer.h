/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _NATDETECTSERVER_H_
#define _NATDETECTSERVER_H_
#include "RNLobby/Inc/BaseServer.h"

namespace RNReplicaNet
{

namespace RNLobby
{

/// A stateless server that responds to query packets with the encoded public IP address of the source.
/// Two instances of this server should be run each on their own separate IP address (in total four IP address:port pairs are used) to allow the NATDetectClient to deduce the NAT type.
/// Assuming a setup with two IP addresses 88.208.234.187 and 88.208.234.188:<br>
/// A multihomed machine with two or more public IP addresses could use: RLSrvNATDetect.exe /i /s -addr 88.208.234.187:3003 -addr2 88.208.234.187:3004 -addr 88.208.234.188:3003 -addr2 88.208.234.188:3004<br>
/// A setup with one public IP address for each machine would need to use two machines: RLSrvNATDetect.exe /i /s -addr 88.208.234.187:3003 -addr2 88.208.234.187:3004<br>RLSrvNATDetect.exe /i /s -addr 88.208.234.188:3003 -addr2 88.208.234.188:3004<br>
class NATDetectServer : public BaseServer
{
public:
	NATDetectServer();
	virtual ~NATDetectServer();

	enum Reply
	{
		kReply_Pending = 0,
		kReply_Success,
		kReply_Timeout
	};
	enum
	{
		kDefaultPort = 4004,
		kDefaultPortOther = 4005
	};

	bool Start(const XPAddress &address = XPAddress(kDefaultPort),const XPAddress &addressOther = XPAddress(kDefaultPortOther));

	virtual void CallbackGotNATDetectProbe(const XPAddress &address) const {};

private:
	bool CallbackParseValidMessage(DynamicMessageHelper &message,const size_t length,const XPAddress &address);

	void CallbackTidy(void);

	t_XPSocket *mOtherSocket;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
