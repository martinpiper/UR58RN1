/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _COMMONSERVER_H_
#define _COMMONSERVER_H_
#include "RNPlatform/Inc/Encryption.h"
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNPlatform/Inc/Thread.h"
#include "RNXPSockets/Inc/XPSockets.h"
#include "RNPlatform/Inc/MessageHelper.h"

namespace RNReplicaNet
{

namespace RNLobby
{

/// A common server support class
class CommonServer
{
public:
	bool CommonCallbackParsePacketData(void *data,size_t &length,DynamicMessageHelper &result,const int moreSalt = 0);
	void CommonPackageAndSendData(const DynamicMessageHelper &message,DynamicMessageHelper &result,const int moreSalt = 0);

	Encryption::Key mCryptoKey;
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
