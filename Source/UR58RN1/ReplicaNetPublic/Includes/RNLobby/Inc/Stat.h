/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _STATSERVER_STAT_H_
#define _STATSERVER_STAT_H_
#include "RNLobby/Inc/UserClient.h"

namespace RNReplicaNet
{

namespace RNLobby
{

namespace StatServerSession
{

// The user must implement these two functions in GCC expected when using BootStatGCC.lang
extern void ProcessPlayer( const int loginID );

extern void ProcessGame( void );


// These functions are actually implemented in SimpleScript by StatGCC.lang
extern int GetResult( const int loginID , const char *name , const int defaultValue );
extern float GetResult( const int loginID , const char *name , const float defaultValue );

extern int GetVariable( const int  loginID , const int section , const UserClient::VariableAccess access , const char *name , const int defaultValue );
extern float GetVariable( const int  loginID , const int section , const UserClient::VariableAccess access , const char *name , const float defaultValue );

extern void SetVariable( const int  loginID , const int section , const UserClient::VariableAccess access , const char *name , const int value );
extern void SetVariable( const int  loginID , const int section , const UserClient::VariableAccess access , const char *name , const float value );

extern void ShiftVariables( const int loginID , const int startSection , const UserClient::VariableAccess access , const char *name , const int numItems , const int sectionOffset);

extern int GetTimeAsSeconds( void );

extern int CalcStatVariable( const int loginID , const int section , const UserClient::VariableAccess access , const char *name , const int defaultValue );
extern float CalcStatVariable( const int loginID , const int section , const UserClient::VariableAccess access , const char *name , const float defaultValue );

extern int CalcStatVariableResult( const int loginID , const int section , const UserClient::VariableAccess access , const char *name , const int defaultValue );
extern float CalcStatVariableResult( const int loginID , const int section , const UserClient::VariableAccess access , const char *name , const float defaultValue );

} // namespace StatServerSession

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
