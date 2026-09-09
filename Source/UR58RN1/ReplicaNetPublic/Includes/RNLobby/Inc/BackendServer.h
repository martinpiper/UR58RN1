/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _BACKENDSERVER_H_
#define _BACKENDSERVER_H_
#include "RNPlatform/Inc/Encryption.h"
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNPlatform/Inc/Thread.h"
#include "RNXPSockets/Inc/XPSockets.h"
#include "RNLobby/Inc/RNLobby.h"
#include "RNLobby/Inc/BaseServer.h"
#include "RNODBCSQL/Inc/ODBCSQL.h"
#include "RNLog/Inc/Log.h"
#include <map>
#include <string>

namespace RNReplicaNet
{

namespace RNLobby
{

/// This class is used to provide a TitleID verification service to BackendClient
/// Backend TitleID information is stored in the following table:
/// - Backend
class BackendServer : public BaseServer
{
public:
	BackendServer(const char *lobbyDSN = MRNLobbyDefaultDSN);
	virtual ~BackendServer();

	enum Reply
	{
		kReply_InvalidTitleID = 0,
		kReply_ValidTitleID,
		kReply_TitleIDSupportPeriodExpired,
		kReply_ServerDownForMaintanence,
		kReply_ServerCannotBeContacted
	};
	enum
	{
		kDefaultPort = 4000,
		kDefaultDebugPort = 4001
	};

	/// Starts the code server thread and processes code queries. If not already started the network hardware is started.
	/// \return Success returns true, failure returns false.
	bool Start(const XPAddress &address = XPAddress(kDefaultPort));

	static std::string TranslateReply(const Reply reply);

	struct TitleIDInfo
	{
		std::string mKey;
		bool mSupportExpired;

		std::string mRLSrvAdvertiseAddress;
		int mRLSrvAdvertisePort;
		std::string mRLSrvAdvertiseCrypto;
		std::string mRLSrvCodeAddress;
		int mRLSrvCodePort;
		std::string mRLSrvCodeCrypto;
		std::string mRLSrvNATAddress;
		int mRLSrvNATPort;
		std::string mRLSrvNATCrypto;
		std::string mRLSrvNATDetect1Address;
		int mRLSrvNATDetect1Port;
		std::string mRLSrvNATDetect2Address;
		int mRLSrvNATDetect2Port;
		std::string mRLSrvNATDetectCrypto;
		std::string mRLSrvStatAddress;
		int mRLSrvStatPort;
		std::string mRLSrvStatCrypto;
		std::string mRLSrvUserAddress;
		int mRLSrvUserPort;
		std::string mRLSrvUserCrypto;
		std::string mRLSrvCloudAddress;
		int mRLSrvCloudPort;
		std::string mRLSrvCloudCrypto;
		std::string mRLSrvUDPRelayAddress;
		int mRLSrvUDPRelayPort;
		std::string mRLSrvUDPRelayCrypto;
	};

private:
	RNReplicaNet::RNLog::Log mLog;

	void CallbackTidy(void);

	/// Called from the scope of the thread but without the class mutex being locked.
	bool CallbackParseValidMessage(DynamicMessageHelper &message,const size_t length,const XPAddress &address);



	class BlockManager : public ThreadClass , public Thread
	{
	public:
		BlockManager(const char *lobbyDSN);
		~BlockManager();

		int ThreadEntry(void);

		void RefreshBlocks(void);

		bool FindTitleIDInfo(const int titleID,TitleIDInfo &info);

	private:
		RNODBCSQL::ODBCSQL mWrite;
		RNODBCSQL::ODBCSQL mRead;

		std::map<int,TitleIDInfo> mInfoList;
	};

	BlockManager mBlockManager;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
