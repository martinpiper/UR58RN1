/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _NONCEGEN_H_
#define _NONCEGEN_H_

namespace RNReplicaNet
{

namespace RNLobby
{

namespace NOnceGen
{

/// Returns a nonce local to this client.
extern int GetNOnce(void);

/// Allows the nonce to be seeded with a known value. The nonce is also seeded by a successful RNLobby::BackendClient verification.
extern void SetNOnce(int nOnce);

} // namespace NOnceGen

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
