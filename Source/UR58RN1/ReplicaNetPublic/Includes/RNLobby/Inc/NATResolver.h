/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _NATSERVER_H_
#define _NATSERVER_H_
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

/// This NAT resolver uses incoming data to respond to queries for NAT negotiation. It can also make requests for NAT
/// negotiation. This class can also make queries to NAT resolve. This class is used by TransportNATUDP.
class NATResolver : private ThreadClass , private Thread , private SysTime
{
public:
	NATResolver();
	virtual ~NATResolver();

	enum
	{
		kDefaultPort = 4002
	};

	/// Starts the NAT resolver thread.
	/// \return Success returns true, failure returns false.
	bool Start(void);

	/// Stops the NAT resolver thread.
	/// \return Success returns true, failure returns false.
	bool Stop(void);

	/// When the NATResolver becomes inactive for a period of time this allows the thread to stop processing.
	void SetAutomaticThreadStop(const bool allow = true);

	/// Allows the host socket set by BeginHosting to be automatically read and all packets received on that socket to be parsed.
	/// Also calls SetAutomaticThreadStop(false)
	void SetAutomaticHostSocketReads(const bool allow = false);

	/// Sets the encryption key to be used by all queries for this server.
	/// \param data The data to use as a seed for the key. This can be NULL to use the default key.
	/// \param length The length of the data to use for creating the key.
	void SetEncryptionKey(const void *data,const int length);

	/// Hosts a NATResolver server instance on this socket. The socket is automatically polled if SetAutomaticHostSocketReads(true) is used.
	/// If the NATResolver thread has not started then this calls Start() first.
	bool BeginHosting(t_XPSocket *socket);

	/// Advertises a socket with a set of identifying numbers (isAdvertised, globalID, sessionID, nonceID) which should be globally unique for this socket.
	// CallbackExternalAddressKnown() can be called.
	/// If the NATResolver thread has not started then this calls Start() first.
	/// \param socket The socket to advertise.
	/// \param userPointer A user pointer passed to NATResolver callbacks.
	/// \param isAdvertised Can be true or false indicating if the socket is advertised with a game server. If being used by TransportNATUDP this detects if AdvertiseClient is advertising a connection.
	/// \param globalID Must a globally unique number, for example a hash of the public IP address. If being used by TransportNATUDP this uses the advertised ID from AdvertiseClient or the currently logged in userID.
	/// \param nonceID Must be a locally unique number, for example the value from NOnceGen::GetNOnce() or the value of the socket pointer cast to an int. If being used by TransportNATUDP this uses the value from NOnceGen::GetNOnce().
	/// \param externalHost If true then this will attempt to ask an external server to help with resolving. The server is set by SetServer() or by default uses the RNLobby NATResolver server.
	/// \param optionalHost An optional host to contact to help with resolving.
	/// \param persistant Keeps the socket alive for longer in the NATResolver index.
	bool BeginAdvertise(t_XPSocket *socket,void *userPointer,const bool isAdvertised,const int globalID,const int sessionID,const int nonceID,const bool externalHost = true,const XPAddress *optionalHost=0,const bool persistant = false);

	/// Starts resolving a connection between this socket and the socket advertised with the set of identifying numbers (isAdvertised, globalID, sessionID, nonceID).
	/// Either CallbackResolveFailed() or CallbackResolveResult() will be called.
	/// If the NATResolver thread has not started then this calls Start() first.
	/// \param socket The socket to resolve.
	/// \param userPointer A user pointer passed to NATResolver callbacks.
	/// \param isAdvertised A value set by BeginAdvertise().
	/// \param globalID A value set by BeginAdvertise().
	/// \param nonceID A value set by BeginAdvertise().
	/// \param tryExternalHost If true then this will attempt to ask an external server to help with resolving. The server is set by SetServer() or by default uses the RNLobby NATResolver server.
	/// \param optionalHost An optional host to contact to help with resolving.
	/// \param optionalFirstTargetTry An optional address to try first, this could be the local LAN IP address.
	bool BeginResolve(t_XPSocket *socket,void *userPointer,const bool isAdvertised,const int globalID,const int sessionID,const int nonceID,const bool tryExternalHost = true,const XPAddress *optionalHost=0,const XPAddress *optionalFirstTargetTry=0);

	/// Removes the socket from the NATResolver index.
	void ForgetSocket(const t_XPSocket *socket);

	/// Forgets all sockets with the same userPointer.
	void ForgetUserPointer(const void *userPointer);

	/// Removes the specific resolve request.
	void ForgetResolve(t_XPSocket *socket,const bool isAdvertised,const int globalID,const int sessionID,const int nonceID);

	/// The user may call this function when a packet is received on a socket that where NAT packets can potentially be received. For example, internally the NATUDP Transport uses a reliable UDP manager which calls a hook function, if it exists after is has been installed by RNLobby::RegisterDefaultTransports(), this function then calls CallbackParsePacketData() for any packets that arrive allowing the extension NATUDP transport to automatically NAT resolve without needed an explicit library dependancy between RNXPURL and RNLobby.
	/// If the NATResolver thread has not started then this calls Start() first.
	/// \return True if the packet is a NAT packet, which implies the packet data doesn't need to be processed by the receiver.
	/// \return False if the packet isn't a NAT packet.
	bool CallbackParsePacketData(t_XPSocket *socket,const XPAddress &addr,void *data,size_t length);

	/// Sets the address and port of the server. The default for the instance of the class is localhost and port NATResolver::kDefaultPort or the address resolved by a successful BackendClient::Start().
	/// \param address The new address and port number to use for the server
	void SetServer(const XPAddress &address);

	/// A virtual callback that is triggered when a NAT resolve attempt fails.
	virtual void CallbackResolveFailed(t_XPSocket *socket,void *userPointer,const int titleID,const bool isAdvertised,const int globalID,const int sessionID,const int nonceID);

	/// A virtual callback that is triggered when a NAT resolve attempt succeeds.
	/// /param targetAddress The target address the socket can send packets to.
	virtual void CallbackResolveResult(t_XPSocket *socket,void *userPointer,const XPAddress &targetAddress,const int titleID,const bool isAdvertised,const int globalID,const int sessionID,const int nonceID);

	/// A virtual callback that is triggered when an advertised socket is told the external address from being advertised to an external host.
	virtual void CallbackExternalAddressKnown(t_XPSocket *socket,void *userPointer,const XPAddress &externalAddress,const int titleID,const bool isAdvertised,const int globalID,const int sessionID,const int nonceID);

	int GetCountBySocket(void);

private:
	int ThreadEntry(void);

protected:

private:
	class NATState;
	class PerHostState;

	bool InsertState(NATState *state);
	void DeleteState(NATState * &state);
	bool PackageAndSendData(t_XPSocket *socket,const DynamicMessageHelper &message,const XPAddress &address);

	Encryption::Key mCryptoKey;
	t_XPSocket *mHost;

	// Compare functors
	struct ltSocket
	{
		bool operator()(const NATState *a,const NATState *b) const;
	};

	struct ltIDs
	{
		bool operator()(const NATState *a,const NATState *b) const;
	};

	struct ltIDsWithSocket
	{
		bool operator()(const NATState *a,const NATState *b) const;
	};

	struct ltUserPointer
	{
		bool operator()(const NATState *a,const NATState *b) const;
	};

	std::multiset<NATState *,ltSocket> mSetBySocket;
	std::set<NATState *,ltIDs> mSetByIDs;				// There must be only one for each ID setting. Used mostly for advertised resolves.
	std::set<NATState *,ltIDsWithSocket> mSetByIDsWithSocket;				// There must be only one for each ID setting.
	std::multiset<NATState *,ltUserPointer> mSetByUserPointer;
	std::list<NATState *> mListByOrder;

	bool BuildAndSendAdvertisePacket(DynamicMessageHelper &message,NATState *state,PerHostState &hostState);

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
