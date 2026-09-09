/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
// Not intended to be compiled directly, but instead intended to be included as common setup code for various simple servers
#ifdef _WIN32
#ifdef _DEBUG
#include <crtdbg.h>
#endif
#endif
#include "RNPlatform/Inc/StrDup.h"
#include "RNPlatform/Inc/Thread.h"
#include "RNPlatform/Inc/ThreadClass.h"
#include "RNLobby/Inc/Service.h"
#include "RNReplicaNet/Inc/ReplicaNet.h"
#include "RNLobby/Inc/RNLobby.h"
#include "RNPlatform/Inc/PlatformHeap.h"
#include "RNPlatform/Inc/MessageHelper.h"
#include "RNXPURL/Inc/TransportConfig.h"
#include <vector>
#include <string>
#if defined(_WIN32)
#include <Winhttp.h>
#pragma comment(lib, "winhttp.lib")
#include "Psapi.h"
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Version.lib")
#endif

#ifndef NAMESPACE
#error Need NAMESPACE defined. This will be the fully qualified class name used by this service host.
#endif
#ifndef SERVICE_LEAF_NAME
#error Need SERVICE_LEAF_NAME defined. This will be the service leaf name registered with the OS.
#endif
#ifndef LONG_DESCRIPTION
#define LONG_DESCRIPTION SERVICE_LEAF_NAME " ReplicaNet RNLobby hosted service"
#endif

#ifndef SUPPRESS_PREFIX_POSTFIX
#if defined(_WIN64)
#define LEAF_POSTFIX ".x64"
#define DESCRIPTION_POSTFIX " (x64)"
#else
#define LEAF_POSTFIX ".x32"
#define DESCRIPTION_POSTFIX " (x32)"
#endif
#endif //< SUPPRESS_PREFIX_POSTFIX


class MyService : public RNReplicaNet::RNLobby::Service
{
public:
	MyService() : Service( SERVICE_LEAF_NAME LEAF_POSTFIX, LONG_DESCRIPTION DESCRIPTION_POSTFIX )
	{
	}
	int Main(int argc = 0,char **argv = 0);

#ifdef EXECUTE_POST_INIT
	void PostInit(NAMESPACE &server,int &argc, char ** &argv);
#endif
#ifdef EXECUTE_CUSTOM_PRINT
	void CustomPrint(std::vector<NAMESPACE*> &servers,RNReplicaNet::SysTime &theTime);
#endif

	std::string GetDebugStatusLine( RNReplicaNet::SysTime &theTime );
};


class ASyncUpdateCheck : public RNReplicaNet::Thread , public RNReplicaNet::ThreadClass
{
public:
	ASyncUpdateCheck()
	{
	}

	virtual ~ASyncUpdateCheck()
	{
		Terminate();
	}

	virtual int ThreadEntry(void)
	{
#if defined(_WIN32)
		// MPi: TODO: Abstract this into an API. On Windows use WinHttpOpen but on Linux use something else, preferably standard.
		// Fetch the service upgrade URL
		wchar_t url[256];
		// An uppercase %S tells swprintf to accept an ANSI string
		swprintf( url , sizeof( url ) , L"test/ServiceUpgradeCheck.asp?Check=1&ServiceName=%S&Version=%d.%d_%S", SERVICE_LEAF_NAME LEAF_POSTFIX , _REPLICANET_VERSION , _RNLOBBY_VERSION , RNReplicaNet::RNLobby::Service::GetFileVersion().c_str() );
		HINTERNET internet = WinHttpOpen( L"_SrvCommon.cpp http://www.replicanet.com/" , WINHTTP_ACCESS_TYPE_NO_PROXY , WINHTTP_NO_PROXY_NAME , WINHTTP_NO_PROXY_BYPASS , 0 );
		HINTERNET connection = WinHttpConnect( internet , L"www.RNLobby.com", INTERNET_DEFAULT_HTTP_PORT , 0);
		HINTERNET request = WinHttpOpenRequest( connection , L"GET" , url , 0 , WINHTTP_NO_REFERER , WINHTTP_DEFAULT_ACCEPT_TYPES , WINHTTP_FLAG_REFRESH | WINHTTP_FLAG_BYPASS_PROXY_CACHE | WINHTTP_FLAG_ESCAPE_PERCENT );
		BOOL ret = WinHttpSendRequest( request , WINHTTP_NO_ADDITIONAL_HEADERS , -1 , WINHTTP_NO_REQUEST_DATA , 0 , WINHTTP_IGNORE_REQUEST_TOTAL_LENGTH , 0 );
		ret = WinHttpReceiveResponse( request , 0 );

		DWORD numRead = 0;
		DWORD GLE = 0;
		DWORD total = 0;
		std::string retStr;
		do
		{
			char buffer[1024];
			ret = WinHttpReadData(request, buffer, sizeof(buffer),&numRead);

			total += numRead;
			GLE = GetLastError();
			//			printf("%s\n" , DynamicMessageHelper::DumpAsHex( buffer , numRead ).c_str() );
			if (numRead)
			{
				retStr += std::string(buffer,numRead);
			}
		} while ( ret && ( numRead > 0 ) );

		WinHttpCloseHandle(request);
		WinHttpCloseHandle(connection);
		WinHttpCloseHandle(internet);
#endif
		RNReplicaNet::RNLobby::Service::DebugOut("Finished update check\n");

		return 0;
	}
};

#ifdef REDIRECT_MAIN
int Redirected_Main(int argc,char **argv)
#else
int main(int argc,char **argv)
#endif
{
#ifdef _WIN32
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_ALLOC_MEM_DF);
#endif
#endif

#ifdef ENABLE_PACKET_SELECT
	RNReplicaNet::TransportConfig::SetEnablePacketSelect(true);
#endif

#ifdef EXECUTE_CUSTOM_SETUP
	if (!CustomSetup())
	{
		RNReplicaNet::RNLobby::Service::DebugOut("CustomSetup returned false\n");
		exit(-1);
	}
#endif

	int origArgc = argc;
	char **origArgv = argv;

	char *username = 0 , *password = 0;

	MyService service;
	RNReplicaNet::RNLobby::Service::DebugOut( "Version: %s\n" , RNReplicaNet::RNLobby::Service::GetFileVersion().c_str() );
	RNReplicaNet::RNLobby::Service::DebugOut( "Entry running as: %s\n" , RNReplicaNet::RNLobby::Service::GetUserInfo().c_str() );

	bool runUpdate = true;
	ASyncUpdateCheck checker;

	int i;
	for (i = 0 ; i < argc ; i++ )
	{
		if (strcmp(argv[i] , "/no-update-check") == 0)
		{
			RNReplicaNet::RNLobby::Service::DebugOut("No update check\n");
			runUpdate = false;
		}
	}

	if (runUpdate)
	{
		checker.Begin(&checker);
	}

	bool handled = false;
	std::vector<std::string> dependencies;

	while (argc-- > 0)
	{
		if ( ( strcmp(*argv,"-h") == 0 ) || ( strcmp(*argv,"/h") == 0 ) || ( strcmp(*argv,"-?") == 0 ) || ( strcmp(*argv,"/?") == 0 ) )
		{
			printf("Service control specific options\n");
			printf("/d - Debug the service. Starts the process as a command line application.\n");
			printf("/i - Install the service and sets the command line parameters of the service.\n");
			printf("/c - Set the command line parameters of the service.\n");
			printf("/s - Start the service.\n");
			printf("/t - Stop the service.\n");
			printf("/u - Uninstall the service.\n");
			printf("/a - Attach main thread as service.\n");
			printf("/p <name> - Add a service name as a dependency. Dependencies must be specified before the /i option.\n");
			printf("/g <Verbose debug file> <Profile debug file> <Error debug file> <file size> - Add debug options for this service, if the output file is empty \"\" then output file debugging is disabled.\n");
			printf("/l <user name> <password> - Use the specified user name and password when installing the service. Must be specified before the /i option.\n");
			printf("/no-update-check - Will disable the update check code.\n");

			printf("Options\n");
#ifdef USES_RNODBCSQL
			printf("-DSN=<ODBC DSN> - This sets the ODBC DSN to use for the next started server. This defaults to \"-" MRNLobbyDefaultDSN "\"\n" );
#endif
			printf("-key=<key> - This sets the key for the next started server.\n" );
			printf("-dstart - Start a server using the default settings available.\n");
#ifdef NATDETECTSERVER
			printf("-addr/-addr2 a.b.c.d:port - Start a server for each address pair.\n");
#else
#ifdef START_WITH_FILENAME
			printf("-file <filename> - Start a server with a filename.\n");
#endif
#ifndef SERVICE_NO_ADDRESS
			printf("-addr a.b.c.d:port - Start a server for each address.\n");
#endif
#endif
#ifdef HAS_AUDITOR
			printf("-auditPath <file path> - Sets the audit path directory.\n");
#endif
#ifdef USES_SECURITY_ROUTE
			printf("-securityRoute <IP address eg. 1.2.3.4> - Sets the IP address to route to when blocking packets.\n");
			printf("-unblockAllNow - Unblocks and removes all IP address blocks.\n");
#endif
			printf("-q - Quiet mode, don't output any regular debug prints each second.\n");
			checker.Terminate();
			exit(0);
		}
		else if (strcmp(*argv,"/a") == 0)
		{
			service.Execute(true,true);
			return 0;
		}
		else if (strcmp(*argv,"/d") == 0)
		{
			*argv = "//";	// Blank out service control switches
			int ret = service.Main(origArgc,origArgv);
			checker.Terminate();
			exit(ret);
		}
		else if (strcmp(*argv,"/i") == 0)
		{
			*argv = "/a";	// Tell service we want to run as a service
			char *pdependencies = 0;
			if (dependencies.size() > 0)
			{
				size_t i;
				size_t totalSize = 1;	// For final null terminate
				for (i=0;i<dependencies.size();i++)
				{
					totalSize+= strlen(dependencies[i].c_str()) + 1;
				}
				pdependencies = (char *)calloc(1,totalSize);
				totalSize = 0;
				for (i=0;i<dependencies.size();i++)
				{
					strcpy(pdependencies+totalSize,dependencies[i].c_str());
					totalSize+= strlen(dependencies[i].c_str()) + 1;
				}
			}
			if ( !service.Create( username , password , true , true , LONG_DESCRIPTION DESCRIPTION_POSTFIX , pdependencies ) )
			{
				printf( "RNReplicaNet::RNLobby::Service::Create failed, probably due to lack of access rights.\n" );
				checker.Terminate();
				exit( -1 );
			}
			if ( !service.SetArgs(origArgc,origArgv) )
			{
				printf( "RNReplicaNet::RNLobby::Service::SetArgs failed, probably due to lack of access rights.\n" );
				checker.Terminate();
				exit( -1 );
			}
			if (pdependencies)
			{
				free(pdependencies);
			}
			handled = true;

			printf( "Service install OK.\n" );
		}
		else if (strcmp(*argv,"/c") == 0)
		{
			*argv = "/a";	// Tell service we want to run as a service
			service.SetArgs(origArgc,origArgv);
			handled = true;
		}
		else if (strcmp(*argv,"/s") == 0)
		{
			*argv = "//";	// Blank out service control switches
			if ( !service.Start() )
			{
				printf( "Service failed to start, has it been installed properly and/or does the user have login as a service rights?\n" );
				checker.Terminate();
				exit( -1 );
			}
			handled = true;
		}
		else if (strcmp(*argv,"/t") == 0)
		{
			*argv = "//";	// Blank out service control switches
			if ( !service.Stop() )
			{
				printf( "Service failed to stop, does this user have correct access rights?\n" );
				checker.Terminate();
				exit( -1 );
			}
			handled = true;
		}
		else if (strcmp(*argv,"/u") == 0)
		{
			*argv = "//";	// Blank out service control switches
			if ( !service.Delete() )
			{
				printf( "RNReplicaNet::RNLobby::Service::Delete failed, probably due to lack of access rights.\n" );
				checker.Terminate();
				exit( -1 );
			}
			printf( "Service uninstall OK.\n" );
			handled = true;
		}
		else if (strcmp(*argv,"/p") == 0)
		{
			argv++;
			argc--;
			if (argc > 0)
			{
				dependencies.push_back(*argv);
			}
			handled = true;
		}
		else if (strcmp(*argv,"/g") == 0)
		{
			if (argc >= 4)
			{
				service.SetDebugOutput( argv[1] , argv[2] , argv[3] , atoi( argv[4] ) );
				argv += 4;
				argc -= 4;
			}
			handled = true;
		}
		else if (strcmp(*argv,"/l") == 0)
		{
			argv[0] = "//";	// Blank out service control switches
			if (argc >= 2)
			{
				username = RNReplicaNet::StrDupA( argv[1] );
				password = RNReplicaNet::StrDupA( argv[2] );
				argv[1] = "//";	// Blank out service username
				argv[2] = "//";	// Blank out service password
				argv += 2;
				argc -= 2;
			}
			handled = true;
		}

		argv++;
	}

	if (handled)
	{
		checker.Terminate();
		exit(0);
	}

	service.DebugOut("No service parameters so execute as the service...\n");
	service.Execute(true,true);

	return 0;
}

static bool sStartedService = false;

int MyService::Main(int argc,char **argv)
{
	if (sStartedService)
	{
		// If by some chance this is called more than once in a process then just exit.
		return 0;
	}
	sStartedService = true;

	int origArgc = argc;
	char **origArgv = argv;
	bool quiet = false;
#ifndef SERVICE_NO_ADDRESS
	RNReplicaNet::XPAddress addr(NAMESPACE::kDefaultPort);
#ifdef NATDETECTSERVER
	RNReplicaNet::XPAddress addr2(NAMESPACE::kDefaultPortOther);
#endif
#endif
#ifdef HAS_AUDITOR
	char *auditPath = 0;
#endif
#ifdef START_WITH_FILENAME
	char *filename = 0;
#endif
#ifdef USES_RNODBCSQL
	const char *theDSN = MRNLobbyDefaultDSN;
#endif
#ifndef SERVICE_NO_ADDRESS
	std::string crypto;
#endif
#ifdef USES_SECURITY_ROUTE
	std::string securityRoute;
#endif
	bool atLeastOneStarted = false;

	RNReplicaNet::RNLobby::Service::DebugOut( "Version: %s\n" , GetFileVersion().c_str() );
	RNReplicaNet::RNLobby::Service::DebugOut( "Service running as: %s\n" , GetUserInfo().c_str() );

	std::vector<NAMESPACE*> servers;

	while (argc--)
	{
		bool startOne = false;
		DebugOut("argv = '%s'\n",*argv);
		if (strcmp(*argv,"-q") == 0)
		{
			quiet = true;
		}
		else if (strcmp(*argv,"-dstart") == 0)
		{
			startOne = true;
#ifdef START_WITH_FILENAME
			filename = 0;
#endif
		}
#ifndef SERVICE_NO_ADDRESS
		else if (strcmp(*argv,"-addr") == 0 && argc > 0)
		{
			argc--;
			argv++;
			addr = RNReplicaNet::XPAddress(*argv);
#ifndef NATDETECTSERVER
			startOne = true;
#endif
		}
#endif
#ifdef HAS_AUDITOR
		else if (strcmp(*argv,"-auditPath") == 0 && argc > 0)
		{
			argc--;
			argv++;
			auditPath = *argv;
		}
#endif
#ifdef NATDETECTSERVER
		else if (strcmp(*argv,"-addr2") == 0 && argc > 0)
		{
			argc--;
			argv++;
			addr2 = RNReplicaNet::XPAddress(*argv);
			startOne = true;
		}
#endif
#ifdef START_WITH_FILENAME
		else if (strcmp(*argv,"-file") == 0 && argc > 0)
		{
			argc--;
			argv++;
			filename = *argv;
			startOne = true;
		}
#endif
#ifdef USES_RNODBCSQL
		else if (strncmp(*argv,"-DSN=", 5) == 0)
		{
			theDSN = RNReplicaNet::StrDup((*argv)+1);
		}
#endif
#ifndef SERVICE_NO_ADDRESS
		else if (strncmp(*argv,"-key=", 5) == 0)
		{
			crypto = std::string((*argv)+5);
		}
#endif
#ifdef USES_SECURITY_ROUTE
		else if (strcmp(*argv,"-securityRoute") == 0 && argc > 0)
		{
			argc--;
			argv++;
			securityRoute = *argv;
		}
		else if (strcmp(*argv,"-unblockAllNow") == 0)
		{
			NAMESPACE *server = new NAMESPACE(theDSN);
			server->RemoveAllBlocksNow();
			delete server;
			exit(0);
		}
#endif

		if (startOne)
		{
			atLeastOneStarted = true;
			startOne = false;
			CreateRegistry();
			if (!quiet)
			{
#ifdef NATDETECTSERVER
				DebugOut( "Addresses used %s   %s\n",addr.Export().c_str(),addr2.Export().c_str());
#else
#ifndef SERVICE_NO_ADDRESS
				DebugOut( "Address used %s\n",addr.Export().c_str());
#else
				DebugOut("Starting one\n");
#endif
#endif
			}

			DebugOut("Allocating\n");
#ifdef USES_RNODBCSQL
			NAMESPACE *server = new NAMESPACE(theDSN);
#else
			NAMESPACE *server = new NAMESPACE;
#endif
			DebugOut("Allocated\n");
#ifdef HAS_AUDITOR
			if (auditPath)
			{
				server->SetAuditPath( auditPath );
			}
			auditPath = 0;
			DebugOut("Set audit path\n");
#endif
#ifdef USES_SECURITY_ROUTE
			server->SetSecurityRoute( securityRoute.c_str() );
#endif

#ifndef SERVICE_NO_ADDRESS
#ifndef SERVICE_NO_CRYPTO_KEY
			if (!crypto.empty())
			{
				server->SetEncryptionKey(crypto.c_str(),crypto.length());
			}
#endif
#endif
#ifdef EXECUTE_POST_INIT
			PostInit(*server,origArgc,origArgv);
#endif

			DebugOut("Starting\n");

#ifdef UDPSERVER
			XPSock_Init();
			RNReplicaNet::t_XPSocket *masterServer = XPSock_UrgentCreate(&addr);
			// Locally hosted NAT server.
			server->SetAutomaticHostSocketReads(true);
			server->BeginHosting(masterServer);
			server->Start();
#else
#ifdef NATDETECTSERVER
			server->Start(addr,addr2);
#else
#ifndef SERVICE_NO_ADDRESS
			server->Start(addr);
#else
#ifdef START_WITH_FILENAME
			if (filename)
			{
				server->Start(filename);
			}
			else
			{
				server->Start();
			}
			filename = 0;
#else
			server->Start();
#endif
#endif
#endif
#endif
			DebugOut("Started\n");

			servers.push_back(server);
		}

		argv++;
	}

	if (!atLeastOneStarted)
	{
#ifdef START_WITH_FILENAME
		DebugOut("No servers started, did you forget to add -dstart or -file to the command line?\n");
#elif !defined(SERVICE_NO_ADDRESS)
		DebugOut("No servers started, did you forget to add -dstart or -addr to the command line?\n");
#else
		DebugOut("No servers started, did you forget to add -dstart to the command line?\n");
#endif
	}

	RNReplicaNet::SysTime theTime;

	while(!NeedToExitMain())
	{
		if (!quiet)
		{
#ifdef EXECUTE_CUSTOM_PRINT
			CustomPrint(servers,theTime);
#else
			DebugPrintNoParams( GetDebugStatusLine( theTime ).c_str() );
#endif


		}
		// Respond quickly (within 100ms) of any request to quit
		int i;
		for (i=0;i<10;i++)
		{
			RNReplicaNet::CurrentThread::Sleep(100);
			if (NeedToExitMain())
			{
				break;
			}
		}
	}

	size_t i;
	for (i=0;i<servers.size();i++)
	{
		servers[i]->Stop();
		delete servers[i];
	}
	servers.clear();

	RNReplicaNet::PlatformHeap::ForceFree();

	return 0;
}

// Returns string with \r
std::string MyService::GetDebugStatusLine( RNReplicaNet::SysTime &theTime )
{
	char buffer[ 1024 ];

	float user,kern;
	RNReplicaNet::RNLobby::Service::GetSystemCPUMetrics(user,kern);

	int ihandlesUsed = 0;
	int ipeakHandlesUsed = 0;
	size_t usedMem = 0;
	size_t peakUsedMem = 0;
#ifdef _WIN32
	DWORD handlesUsed = 0;
	static DWORD peakHandlesUsed = 0;

	PROCESS_MEMORY_COUNTERS memory;
	BOOL bret = GetProcessHandleCount( GetCurrentProcess() , &handlesUsed );
	if ( bret )
	{
		if ( handlesUsed > peakHandlesUsed )
		{
			peakHandlesUsed = handlesUsed;
		}
	}
	ihandlesUsed = (int) handlesUsed;
	ipeakHandlesUsed = (int) peakHandlesUsed;

	GetProcessMemoryInfo( GetCurrentProcess() , &memory , sizeof( memory ) );
	usedMem = (size_t) memory.WorkingSetSize;
	peakUsedMem = (size_t) memory.PeakWorkingSetSize;

#endif

	RNReplicaNet::SysTimeType theTimeNow = theTime.FloatTime();

#ifndef SERVICE_NO_ADDRESS
	XPSock_UpdatePerSecondMetrics();
	sprintf( buffer , "secs %.1f mins %.1f hrs %.1f days %.1f got %d sent %d gsize %d ssize %d sckgtsiz/s %.2f sckstsiz/s %.2f cpu%.2f%% krn%.2f%% hand %d peakHand %d usedMem %d(%dK) peakMem %d(%dK)\r"
		,theTimeNow
		,theTimeNow/60.0f
		,theTimeNow/60.0f/60.0f
		,theTimeNow/60.0f/60.0f/24.0f
		,(int)asocket_packets_got , (int)asocket_packets_sent , (int)asocket_packets_got_size , (int)asocket_packets_sent_size
		,asocket_packets_got_size_per_second , asocket_packets_sent_size_per_second
		,user , kern
		,(int)ihandlesUsed , (int)ipeakHandlesUsed
		,(int)usedMem , (int)(usedMem/1024)
		,(int)peakUsedMem , (int)(peakUsedMem/1024)
		);
#else
	sprintf( buffer , "secs %.1f mins %.1f hrs %.1f days %.1f cpu %.2f%% krn %.2f%% hand %d peakHand %d usedMem %d(%dK) peakMem %d(%dK)\r"
		,theTimeNow
		,(theTimeNow/60.0f)
		,(theTimeNow/60.0f/60.0f)
		,(theTimeNow/60.0f/60.0f/24.0f)
		,user , kern
		,(int)ihandlesUsed , (int)ipeakHandlesUsed
		,(int)usedMem , (int)(usedMem/1024)
		,(int)peakUsedMem , (int)(peakUsedMem/1024)
		);
#endif

	return buffer;
}
