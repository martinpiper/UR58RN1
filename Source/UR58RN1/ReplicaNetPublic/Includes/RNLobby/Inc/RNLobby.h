/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _RNLOBBY_H_
#define _RNLOBBY_H_

#define	_RNLOBBY_VERSION 1200

#ifndef MRNLobbyDefaultDSN
#define MRNLobbyDefaultDSN "DSN=RNLobby"
#endif

namespace RNReplicaNet
{

namespace RNLobby
{

/**
 * This static member function registers all of the default transports used by RNLobby.
 */
extern void RegisterDefaultTransports(void);

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
