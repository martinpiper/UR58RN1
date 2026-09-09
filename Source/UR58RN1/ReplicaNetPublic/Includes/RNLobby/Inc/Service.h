/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _SERVICE_H_
#define _SERVICE_H_
#include <string>
#include <list>

namespace RNReplicaNet
{

namespace RNLobby
{

/// Implements a service. Only one instance of this class may be allocated in a process.
/// On Windows platforms the service is executed by the service scheduler.
/// On Linux platforms the service runs as a separate process.
class Service
{
public:
	/// Initialises this service with the parameters.
	/// \param internalServiceName If there are different platform builds of the same service it is a good idea, but not mandatory, to postfix internalServiceName with ".x32" or ".x64". The _SrvCommon.cpp file does this automatically.
	Service(const char *internalServiceName,const char *displayServiceName);

	virtual ~Service();

	/// Sets the debug printing names without instantiating this class.
	static void SetDebugNames(const char *internalServiceName,const char *displayServiceName);
	static bool SetDebugOutput( const char *verboseDebugFile , const char *profileDebugFile , const char *errorDebugFile , const int fileSize );
	/// Sets the registry values for this service
	/// \param userModifiableKey By default this is false and will access keys in the "Replica Software\Managed Services" service configuration area. When true this will access keys in the "User Modifiable" sub-key that has full access for the installed service user account.
	static void SetRegistry(const char *name , const char *value , const bool userModifiableKey = false );
	static void SetRegistry(const char *name , const int value , const bool userModifiableKey = false );
	/// Gets the registry values for this service. If it fails value is left unchanged.
	/// \param userModifiableKey See SetRegistry for an explanation of this parameter.
	static bool GetRegistry(const char *name , std::string &value , const bool userModifiableKey = false);
	static bool GetRegistry(const char *name , int &value , const bool userModifiableKey = false);

	/// Creates and installs a service using this process.
	/// \param userName Optional user name (account) to use for this service installation. Otherwise the default local system account will be used.
	/// \param password The password for the userName
	/// \param autoStart When true the service will automatically start when the operating system starts.
	/// \param restartOnError If true and the Main function generates an exception this will restart the service.
	/// \param longDescription Optional human readable text description for this service.
	bool Create(const char *userName = 0,const char *password = 0,const bool autoStart = true,const bool restartOnError = true,const char *longDescription = 0, const char *dependencies = 0);

	/// Sets the arguments to use for this service. On Windows platforms these are stored in the registry.
	static bool SetArgs(int argc = 0,char **argv = 0);

	/// Gets the arguments to use for this service. On Windows platforms these are stored in the registry.
	static bool GetArgs(int &argc,char ** &argv);

	/// Starts the service.
	bool Start(void);

	/// Stops the service.
	bool Stop(void);

	/// Deletes the service.
	bool Delete(void);

	/// Call this to start running the application as a service
	/// \param setCurrentDirectory When true the service will set the current process directory to be the directory of the executable. This is to allow compatibility with processes that expect the current directory to be the execution directory. When false and running under Windows the current directory will most likely be something similar to %WINDIR%\systemXX depending on the OS version.
	/// \param storeLastDebugPrint When true this stores the last debug print line in the registry. Useful as an aid to debugging.
	bool Execute(const bool setCurrentDirectory = true,const bool storeLastDebugPrint = false);

	/// The main work function which must be implemented.
	///	Any unhandled exception (such as accessing an invalid memory address) will be caught by this Service class and will usually cause the service to restart if configured.
	virtual int Main(int argc = 0,char **argv = 0) = 0;

	/// A utility function that allows the user to output debug information as the service is running. This debug information is stored for display by RLSrvMonitor so do not use this to display confidential information.
	static void DebugPrint(const char *, ...);
	/// Same as DebugPrint except that the input text has no parameter parsing so it uses slightly less CPU time and memory.
	static void DebugPrintNoParams(const char *);

	/// A utility function that allows the user to output debug information as the service is running. This gets set into the RLSrvMonitor/RLServerMonitor accessible debug line and should not be used for confidential information.
	static void SetDebugLine(const char *buffer);

	/// Retrieves the last line from DebugPrint for the current service.
	static bool GetDebugLine(std::string &buffer);

	/// Retrieves the last line from DebugPrint for a service name.
	/// If the service name is postfixed with ".x32" or ".x64" the relevant service is queried, otherwise the service matching the calling process type is queried.
	static bool GetDebugLine(const char *serviceName,std::string &buffer);

	/// Gets the list of service names that are installed.
	/// Can return the service name prefixed with "x32." or "x64." if the service is registered to a different process type.
	static bool GetServiceList(std::list<std::string> &services);

	/// A utility function that allows the user to output debug information as the service is running. This does not get set into the RLSrvMonitor/RLServerMonitor accessible debug line and can be used for more confidential information.
	static void DebugOut(const char *, ...);

	/// A utility function that allows the user to output debug information to a file.
	static void DebugPrintFile(const char *text);

	// Gets CPU metrics for this process.
	static void GetSystemCPUMetrics(float &userPercent, float &kernelPercent);

	// Gets the configured internal service name.
	static const char *GetInternalServiceName(void);

	// Gets the configured display service name.
	static const char *GetDisplayServiceName(void);

	static std::string GetFileVersion(void);
	static std::string GetUserInfo(void);

protected:
	/// Returns true if the Main function needs to exit. This must be tested fairly regularly by the Main function.
	bool NeedToExitMain(void);

	static void CreateRegistry(void);

	static void DebugPrintCommon( char *buffer );
	static void DebugPrintCommonStore( char *buffer );
private:
};

} // namespace RNLobby

} // namespace RNReplicaNet

#endif
