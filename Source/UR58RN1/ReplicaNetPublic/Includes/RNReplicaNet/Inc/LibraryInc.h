#ifdef _WIN64
#ifdef _DEBUG
#pragma comment( lib , "x64/RNReplicaNetDM.lib" )
#else
#pragma comment( lib , "x64/RNReplicaNetRM.lib" )
#endif
#else
#ifdef _DEBUG
#pragma comment( lib , "RNReplicaNetDM.lib" )
#else
#pragma comment( lib , "RNReplicaNetRM.lib" )
#endif
#endif
