/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _RNLOBBY_SECURITY_H_
#define _RNLOBBY_SECURITY_H_
#include "RNODBCSQL/Inc/ODBCSQL.h"
#include "RNODBCSQL/Inc/Prepare.h"
#include "RNLog/Inc/Log.h"
#include "RNXPSockets/Inc/XPSockets.h"

namespace RNReplicaNet
{

namespace RNLobby
{

/// This class implements a security service that will detect typical intrusion attempts and mitigate them:
/// - Windows server multiple failed USER logons from remote addresses are blocked by updating the routing table.
/// Information is stored in the following database tables:
/// - IPBlockListFromLoginFail
class Security : public ThreadClass , public Thread
{
public:
	Security(const char *lobbyDSN);
	virtual ~Security();

	bool Start(void);

	bool Stop(void);

	int GetTotalNumBlocked( void );

	void SetSecurityRoute( const char *securityRoute );
	void SetMaxCurrentBlocks( const int maxCurrentBlocks = 1000 );

	static int GetNumBlockedThisSession(void);

	void RemoveAllBlocksNow(void);

protected:
	int ThreadEntry(void);

private:
	void ThreadExiting(const int returnCode);

	RNReplicaNet::RNLog::Log mLog;
	RNReplicaNet::RNODBCSQL::ODBCSQL mRead , mWrite;
	RNReplicaNet::RNODBCSQL::Prepare mPreparedInsertIPBlock;
	RNReplicaNet::RNODBCSQL::Prepare mPreparedDeleteIPBlock;

	std::string mSecurityRoute;
	size_t mMaxCurrentBlocks;

	void AddOrRemoveRouting( const XPAddress &addr , const bool add , const std::string &naughtyIP , const std::string &textBlock , const std::string &triedLogin );
};


} // namespace RNLobby

} // namespace RNReplicaNet

#endif
