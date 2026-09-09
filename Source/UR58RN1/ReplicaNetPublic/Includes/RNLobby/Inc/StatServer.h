/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _STATSERVER_H_
#define _STATSERVER_H_
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNLobby/Inc/RNLobby.h"
#include "RNLobby/Inc/BaseTransportServer.h"
#include "RNXPSockets/Inc/XPSocket.h"
#include "RNODBCSQL/Inc/ODBCSQL.h"
#include "RNODBCSQL/Inc/Prepare.h"
#include "RNLog/Inc/Log.h"
#include <map>

namespace RNReplicaNet
{

namespace RNLobby
{

/// Handles requests for statistics
/// Statistics server specific information is stored in the following database tables:
/// - StatAdminUsers
class StatServer : public BaseTransportServer
{
public:
	StatServer(const char *lobbyDSN = MRNLobbyDefaultDSN);
	virtual ~StatServer();

	enum
	{
		kDefaultPort = 4010
	};

	/// Because this uses a TransportUDP connection it will call XPURL::RegisterDefaultTransports()
	bool Start(const XPAddress &address = XPAddress(kDefaultPort));

	/// Returns the total number of stats sessions.
	size_t GetNumTotal(void) const;

	struct LoginIDData
	{
		std::map<std::string,std::string> mValuePairs;
	};
	struct SessionData
	{
//		std::string mSessionNonce;
		std::map<int,LoginIDData> mDataByLoginID;
		bool mClientOKToUpload;
		int mGettingScriptID;
		int mGettingTitleID;
		bool mSessionStarted;
		int mSessionScriptID;
		int mSessionTitleID;
		DynamicMessageHelper mScriptData;
		int mCapturingLoginIDPairs;
	};

private:
	RNReplicaNet::RNLog::Log mLog;

	RNODBCSQL::ODBCSQL mWrite;
	RNODBCSQL::ODBCSQL mRead;
	std::map<Transport *,SessionData> mMapSessionDataByTransport;
	MutexClass mMapByTransportLock;

	size_t mNumActive;
	size_t mNumTotal;

	RNODBCSQL::Prepare mPreparedCountStatAdminUsersByIDForTitleID;

	bool CallbackPoll(void);

	bool CallbackParseValidMessage(Transport *transport,DynamicMessageHelper &message,const size_t length);

	void CallbackAccepted(Transport *transport);

	void CallbackDisconnected(Transport *transport);
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
