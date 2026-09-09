/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __XBOXUDP_H__
#define __XBOXUDP_H__

#include <string>

namespace RNReplicaNet
{

namespace XboxUDP
{

/// This creates an Xbox specific URL from an XNADDR and ReplicaNet channel number
/// \param targetAddr The XNADDR of the target Xbox.
/// \param channel The ReplicaNet game channel set by ReplicaNet::SetGameChannel() or XPSession::SetGameChannel().
/// \return The URL to use.
extern std::string CreateSessionURL(const XNADDR &targetAddr,const int channel);

/// This creates an Xbox specific URL from an XNADDR and Transport channel number
/// \param otherAddr The XNADDR of the target Xbox.
/// \param channel The channel used by target Xbox Transport::Listen().
/// \return The URL to use.
extern std::string CreateTransportURL(const XNADDR &otherAddr,const int channel);

/// This sets the Xbox XNKID and XNKEY to be used by ReplicaNet.
extern void UseKey(const XNKID &sessionID,const XNKEY &keyExchangeKey);

/// Allow automatic use of XNetUnregisterInAddr()
extern void AllowXNetUnregisterInAddr(const bool allow = true);

} // namespace XboxUDP

} // namespace RNReplicaNet

#endif
