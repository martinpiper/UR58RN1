/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _UTILITYSERVER_H_
#define _UTILITYSERVER_H_
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNPlatform/Inc/Thread.h"
#include "RNLobby/Inc/RNLobby.h"
#include "RNXPSockets/Inc/XPSocketClass.h"
#include "RNODBCSQL/Inc/ODBCSQL.h"
#include "RNLog/Inc/Log.h"
#include "RNWeb/Inc/Daemon.h"
#include "RNWeb/Inc/Worker.h"
#include <list>

namespace RNReplicaNet
{

namespace RNLobby
{

class UtilityServerWorker;

/// Handles requests for utility functions using RESTful HTTP. This is not a client visible server hence there is no corresponding UtilityClient class.
class UtilityServer : public RNWeb::Daemon
{
public:
	UtilityServer(const char *lobbyDSN = MRNLobbyDefaultDSN);
	virtual ~UtilityServer();

	enum
	{
		kDefaultPort = 2000
	};

	bool Start(const XPAddress &address = XPAddress(kDefaultPort))
	{
		return RNWeb::Daemon::Start(address);
	}

	virtual RNWeb::Worker *CreateWorker(XPSocket *accepted);

private:
friend class UtilityServerWorker;
	RNReplicaNet::RNLog::Log mLog;

	RNODBCSQL::ODBCSQL mWrite;
	RNODBCSQL::ODBCSQL mRead;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
