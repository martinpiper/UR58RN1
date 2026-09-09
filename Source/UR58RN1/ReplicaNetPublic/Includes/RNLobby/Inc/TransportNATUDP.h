/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __TRANSPORTNATUDP_H__
#define __TRANSPORTNATUDP_H__
#include "RNXPURL/Inc/Transport.h"
#include "RNXPSockets/Inc/XPSocket.h"

namespace RNReplicaNet
{

namespace RNLobby
{

/**
 * A transport class for the NATUDP type of URL. Internally this extends the UDP URL.
 *<br>
 * The exported URL is encoded to contain NAT information required to find machines located anywhere on the local network or the internet using an external NATResolver server.
 */
class TransportNATUDP : public Transport
{
public:
	/**
	 * The ctor performs some basic initialisation
	 */
	TransportNATUDP();

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~TransportNATUDP();

	// Optional implementation of Transport virtual functions.
	int GetPacketsSent(const bool reset);

	int GetPacketsSentMerged(const bool reset);

	int GetPacketsSentSize(const bool reset);

	int GetPacketsReceived(const bool reset);

	int GetPacketsReceivedMerged(const bool reset);

	int GetPacketsReceivedSize(const bool reset);

	int GetPacketsLost(const bool reset);

	void SetBandwidthLimit(const Bandwidth enable);

	Bandwidth GetBandwidthLimit();

	void SetMaximumOutputBandwidth(const int bytesPerSecond);

	int GetMaximumOutputBandwidth(void);

	void SetMaximumInputBandwidth(const int bytesPerSecond);

	int GetMaximumInputBandwidth(void);

	std::string GetPeerURL(void);

	bool GetTransportClosed(void);

	void SetEnableLANDiscovery(const bool enable);

	bool GetEnableLANDiscovery(void);

	// These pure virtual functions are the minimum feature set the transport has to support.

	std::string CanAccept(void);

	std::string CanRoute(void);

	std::string ExportURL(void);

	Transport *Allocate(void);

	Error Listen(const int channel);

	Error Listen(const std::string &address);

	void CommonListen(void);

	Error Connect(const std::string address);

	Transport *Accept(void);

	Error Send(const char *data,const int len);

	Error SendOrdered(const char *data,const int len,const unsigned int band = 0);

	Error SendReliable(const char *data,const int len,const unsigned int band = 0);

	Error SendCertain(const char *data,const int len);

	int Recv(char *const data,const int maxlen);

	int GetMaxPacketSize(void);

	std::string HandleDiscovery(void);

	void HandleDiscoveryReply(const std::string reply);

	void BeginDiscovery(void);

	std::string GetNextDiscovery(void);

	Error GetStatus(void);

	void SetBaseTransport(Transport *baseTransport);

	// Extension API for this transport type. Normally this wouldn't need to be used.
	/// This sets the NAT values to be used for this transport instance.
	/// These values are set at construction time from the internal lobby state and may be changed at any time before
	/// Connect() or Listen() are used.
	/// \return Success returns true, failure returns false.
	bool SetNATValues(const bool isAdvertised,const int globalID,const int sessionID);
	void GetNATValues(bool &isAdvertised,int &globalID,int &sessionID);

	/// This sets the NATResolver server address for any NATUDP transports. This must be called before any NATUDP transports have been registered by RNLobby::RegisterDefaultTransports().
	/// The default is to have no server set and therefore the default server is configured by the internal lobby state from any successful Backend verification. PlatformHeap::ForceFree() will also clear any server that has been set.
	static void SetServer(const XPAddress &address);
	/// Clears any server set by SetServer().
	static void ClearServer(void);


private:
friend class MyNATResolver;
	void SetError(void);

	bool mAllocated;
	Transport *mBaseTransport;
	bool mIsResolved;
	int mMyNOnceUsed;
	bool mKnowExternalAddress;
	XPAddress mExternalAddress;
	bool mIsListen;
	bool mIsUsed;

	bool mStateIsAdvertised;
	int mStateGlobalID;
	int mStateSessionID;
//	bool mBeginResolveCalled;
//	SysTime mBeginResolveCalledTime;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
