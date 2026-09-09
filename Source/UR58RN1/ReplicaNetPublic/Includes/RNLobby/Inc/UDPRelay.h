/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _UDPRELAYSERVER_H_
#define _UDPRELAYSERVER_H_
#include "RNPlatform/Inc/MessageHelper.h"
#include "RNPlatform/Inc/Encryption.h"
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNPlatform/Inc/Thread.h"
#include "RNXPSockets/Inc/XPSocket.h"
#include "RNPlatform/Inc/SysTime.h"
#include <set>
#include <list>

namespace RNReplicaNet
{

namespace RNLobby
{

/// This UDP relay forwards packets to requested clients
class UDPRelay : private ThreadClass , private Thread , private SysTime
{
public:
	UDPRelay();
	virtual ~UDPRelay();

	enum
	{
		kDefaultPort = 4011
	};

	/// Starts the UDP relay thread.
	/// \return Success returns true, failure returns false.
	bool Start(void);

	/// Stops the UDP relay thread.
	/// \return Success returns true, failure returns false.
	bool Stop(void);

	/// Allows the host socket set by BeginHosting to be automatically read and all packets received on that socket to be parsed.
	void SetAutomaticHostSocketReads(const bool allow = false);

	/// Sets the encryption key to be used by all queries for this server.
	/// \param data The data to use as a seed for the key. This can be NULL to use the default key.
	/// \param length The length of the data to use for creating the key.
	void SetEncryptionKey(const void *data,const int length);

	/// Hosts a UDPRelay server instance on this socket. The socket is automatically polled if SetAutomaticHostSocketReads(true) is used.
	/// If the UDPRelay thread has not started then this calls Start() first.
	bool BeginHosting(t_XPSocket *socket);

	/// Advertises a socket with a set of identifying numbers (isAdvertised, globalID, sessionID, nonceID) which should be globally unique for this socket. This may be called as often as needed, to maintain an active session.
	// CallbackExternalAddressKnown() can be called.
	/// If the UDPRelay thread has not started then this calls Start() first.
	/// \param socket The socket to advertise.
	/// \param userPointer A user pointer passed to UDPRelay callbacks.
	/// \param isAdvertised Can be true or false indicating if the socket is advertised with a game server. If being used by TransportNATUDP this detects if AdvertiseClient is advertising a connection.
	/// \param globalID Must a globally unique number, for example a hash of the public IP address. If being used by TransportNATUDP this uses the advertised ID from AdvertiseClient or the currently logged in userID.
	/// \param nonceID Must be a locally unique number, for example the value from NOnceGen::GetNOnce() or the value of the socket pointer cast to an int. If being used by TransportNATUDP this uses the value from NOnceGen::GetNOnce().
	/// \param timeoutData The maximum time when no packet data is sent (using SendTo) or received for this request, after which the request is removed.
	/// \param timeoutSession The maximum expected session time. Repeated calls to Advertise() will reset this timeout.
	bool Advertise(t_XPSocket *socket,void *userPointer,const bool isAdvertised,const int globalID,const int sessionID,const int nonceID,const float timeoutData = 30.0f,const float timeoutSession = 600.0f);

	/// Sends data from this socket to the socket advertised with the set of identifying numbers (isAdvertised, globalID, sessionID, nonceID).
	/// \param socket The socket to resolve.
	/// \param isAdvertised A value set by BeginAdvertise().
	/// \param globalID A value set by BeginAdvertise().
	/// \param nonceID A value set by BeginAdvertise().
	/// \param datain The pointer to data to send.
	/// \param lenin The length of the data, in bytes, to send.
	bool SendTo(t_XPSocket *socket,const bool isAdvertised,const int globalID,const int sessionID,const int nonceID,const char *datain,const int lenin);

	/// Removes the socket from the UDPRelay index.
	void ForgetSocket(const t_XPSocket *socket);

	/// Forgets all sockets with the same userPointer.
	void ForgetUserPointer(const void *userPointer);

	/// The user may call this function when a packet is received on a socket that where UDP relay packets can potentially be received.
	/// If the UDPRelay thread has not started then this calls Start() first.
	/// \return True if the packet is a UDP relay packet, which implies the packet data doesn't need to be processed by the receiver.
	/// \return False if the packet isn't a UDP relay packet.
	bool ParsePacketData(t_XPSocket *socket,const XPAddress &addr,void *data,size_t length);

	/// A virtual callback that is triggered when packet data is received from the UDP relay.
	/// \return True if the packet is a UDP relay packet, which implies the packet data doesn't need to be processed by the receiver.
	/// \return False if the packet isn't a UDP relay packet.
	virtual void CallbackDataReceived(t_XPSocket *socket,const XPAddress &addr,void *data,size_t length);

	/// Sets the address and port of the server. The default for the instance of the class is localhost and port UDPRelay::kDefaultPort or the address resolved by a successful BackendClient::Start().
	/// \param address The new address and port number to use for the server
	void SetServer(const XPAddress &address);

	/// A virtual callback that is triggered when an advertised socket is told the external address from being advertised to an external host.
	virtual void CallbackExternalAddressKnown(t_XPSocket *socket,void *userPointer,const XPAddress &externalAddress,const int titleID,const bool isAdvertised,const int globalID,const int sessionID,const int nonceID);

	int GetCountBySocket(void);

private:
	int ThreadEntry(void);

protected:

private:
	class UDPRelayState;
	class PerHostState;

	bool InsertState(UDPRelayState *state);
	void DeleteState(UDPRelayState * &state);
	bool PackageAndSendData(t_XPSocket *socket,const DynamicMessageHelper &message,const XPAddress &address);

	Encryption::Key mCryptoKey;
	t_XPSocket *mHost;

	// Compare functors
	struct ltSocket
	{
		bool operator()(const UDPRelayState *a,const UDPRelayState *b) const;
	};

	struct ltIDs
	{
		bool operator()(const UDPRelayState *a,const UDPRelayState *b) const;
	};

	struct ltIDsWithSocket
	{
		bool operator()(const UDPRelayState *a,const UDPRelayState *b) const;
	};

	struct ltUserPointer
	{
		bool operator()(const UDPRelayState *a,const UDPRelayState *b) const;
	};

	std::multiset<UDPRelayState *,ltSocket> mSetBySocket;
	std::set<UDPRelayState *,ltIDs> mSetByIDs;				// There must be only one for each ID setting. Used mostly for advertised UDP relays.
	std::set<UDPRelayState *,ltIDsWithSocket> mSetByIDsWithSocket;				// There must be only one for each ID setting.
	std::multiset<UDPRelayState *,ltUserPointer> mSetByUserPointer;
	std::list<UDPRelayState *> mListByOrder;

	bool BuildAndSendAdvertisePacket(DynamicMessageHelper &message,UDPRelayState *state,PerHostState &hostState);

	bool mIsEmpty;
	SysTimeType mBecameEmptyAt;
	bool mAllowAutoThreadStop;
	bool mAllowAutoHostSocketReads;

	XPAddress mServerAddress;
	bool mServerAddressSet;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
