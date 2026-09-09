/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _BACKENDCLIENT_H_
#define _BACKENDCLIENT_H_
#include "RNPlatform/Inc/Encryption.h"
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNPlatform/Inc/Thread.h"
#include "RNXPSockets/Inc/XPSockets.h"
#include "RNLobby/Inc/BackendServer.h"

namespace RNReplicaNet
{

namespace RNLobby
{

/// This backend client can query a backend server to verify the title ID is valid, or can be used to program the rest of the
/// interfaces used by RNLobby.
/// <br>When using this class to verify a titleID BackendClient::GetReply() must successfully return BackendServer::kReply_ValidTitleID
/// before any other RNLobby client classes are started with their Start() member functions.
/// This is because BackendClient will set the correct server address and encryption key to use for each component service when
/// the component is constructed.
class BackendClient : public BaseServer
{
public:
	BackendClient();
	virtual ~BackendClient();

	/// Starts a thread to verify the supplied title ID and encryption key.
	/// Using title ID 0 (zero) with any encryption key is a special case and will allow locally hosted servers to be used for debugging purposes.
	/// \return Success returns true, failure returns false.
	bool Start(const int titleID,const char *encryptionKey);

	/// Tests the completion state of the last query. When the query completes the result can be retrieved by using GetReply()
	/// \return A completed query returns true, a pending query returns false.
	bool GetCompleted(void);

	/// Returns the last reply from a completed query.
	/// \return The last reply from a completed query.
	BackendServer::Reply GetReply(void) const;

	virtual void CallbackCompleted(void) {}

	/// \return Success returns true, failure returns false.
	virtual bool CallbackServerAddress(XPAddress &address);

	/// In debug mode an assert will be generated if BackendClient is used after any other RNLobby service.
	/// To disable this check for the cases where LAN mode (without internet verification) is used before connecting to the internet please call this function.
	static void DisableValidateOrderingCheck(void);

	/// When the BackendClient has successfully completed an authentication request this returns true.
	static bool IsAuthenticated(void);

	/// Sets BackendClient to query the debug or release servers for server configuration information. For development use the debug servers. For public releases use the release servers.
	/// \param use Set to true to use the release servers. By default the debug servers are used.
	void UseReleaseServers(const bool use = false);

private:
	bool CallbackParseValidMessage(DynamicMessageHelper &message,const size_t length,const XPAddress &address);

	void CleanQuery(void);

	bool CallbackPoll(void);

	BackendServer::Reply mReply;
	int mTitleID;
	std::string mEncryptionKey;

	XPAddress mAddress;

	SysTimeType mLastSentTime;
	int mTries;
	bool mAddressGot;

	bool mUseReleaseServers;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
