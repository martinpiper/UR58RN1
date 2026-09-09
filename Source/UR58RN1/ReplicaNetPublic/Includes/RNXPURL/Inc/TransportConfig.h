/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _TRANSPORTCONFIG_H_
#define _TRANSPORTCONFIG_H_
#include <string>

namespace RNReplicaNet
{

const int kUDPIPHeaderBytes = 42;	// The number of bytes used by the network stack IP header information on the wire for UDP packets

/**
 * This class provides low-level access methods for the Transport mechanism.
 * These values must not be changed once XPURL (or XPSession or ReplicaNet) start to send packets.
 */
class TransportConfig
{
public:
	/**
	 * This allows the delay between reliable packets being resent due to failed ACKs to be altered. These options are provided for advanced users to tweak. Changing these settings can make your XPURL derived network connection unstable, care must be taken.
	 * This configuration option remains in force for the lifetime of the application until a new function call is made.
	 * \param numberOfTimes The number of times a packet is resent before finally being given up as lost. The maximum number of times that can be configured is 60.
	 * \param retryTimes The interval in seconds between each resend of a packet.
	 */
	static void SetResendTimes(const int numberOfTimes,const float *retryTimes);

	/**
	 * Allows the resend time delays to be read.
	 * \param numberOfTimes A pointer to an int to contain the number of times a packet is resent.
	 * \param retryTimes If not null an array of floats large enough to accept the resend times.
	 */
	static void GetResendTimes(int *numberOfTimes,float *retryTimes);

	/**
	 * This allow the packet buffer time to be configured for each Transport. The larger the buffer time the more individual packets can be merged into one larger packet before sending. The default value is 0.01f which means this feature is enabled.
	 * This configuration option remains in force for the lifetime of the application until a new function call is made.
	 * <br>For example:
\verbatim
	RNReplicaNet::TransportConfig::SetPacketBufferTime(1.0f / 100.0f);
	RNReplicaNet::TransportConfig::SetPacketBufferEnable(true);
\endverbatim
	 * \param maxTime The time for the packet buffer. The default value is 0.01f which is 1.0f / 100.0f.
	 */
	static void SetPacketBufferTime(const float maxTime = 0.01f);

	/**
	 * Allow the packet buffer time to be read.
	 * \return The current packet buffer time.
	 */
	static float GetPacketBufferTime(void);

	/**
	 * This allows the packet buffer time window to be fine tuned by allowing the application to start buffering all sent packets,
	 * regardless of the time specified by SetPacketBufferTime(). Packets will only be sent when the buffers become full.
	 * A matching call to PacketBufferEndCapture() must be made to signify when normal packet buffering configured by
	 * SetPacketBufferTime() is resumed.
	 */
	static void PacketBufferBeginCaptureWindow(void);

	/**
	 * This is the matching call to PacketBufferBeginCapture() and signifies the end of the packet buffer window.
	 * \param kickBuffer If true the currently buffered packets will be sent by this function call. If false the currently buffered packets will wait until the time configured by SetPacketBufferTime()
	 */
	static void PacketBufferEndCaptureWindow(const bool kickBuffer = true);

	/**
	 * This can override all other packet buffering settings. If all packet buffering is disabled then this will save memory allocated for potential packet buffers in the future.
	 * This is because the PacketBufferBeginCaptureWindow() could be used at any point in the application and to allow this mechanism to work the transport needs to be warned about there being possible buffered packets. This function must not be used when the network has been started.
	 * The default is to enable packet buffering and allocate memory for potential packet buffers. SetPacketBufferEnable(true) should be used if any packet buffers are configured for potential use.
	 * This configuration option remains in force for the lifetime of the application until a new function call is made.
	 * \param disable If true the memory for packet buffers is allocated. If false memory for packet buffering is not allocated.
	 */
	static void SetPacketBufferEnable(const bool enable = true);

	/**
	 * Allows the setting for SetPacketBufferEnable() to be read.
	 * \return The setting for SetPacketBufferEnable()
	 */
	static bool GetPacketBufferEnable(void);

	/**
	 * Allows the MTU of packets to be set when using packet buffering. ReplicaNet V5.4 and earlier used an MTU of 554 bytes (512 bytes plus kUDPIPHeaderBytes).
	 * To improve performance in the majority of cases the MTU has been increased to 1306, which results in a payload of 1264 bytes plus kUDPIPHeaderBytes. This reduces the number of separate packets being sent over the wire.
	 * The value of 1264 has been derived as the lowest common denominator used by various games consoles and operating systems.
	 * \param bytes The number of bytes on the wire to use as an MTU. The minimum value is 554 (TransportMinimumPacketSize plus kUDPIPHeaderBytes). In most cases the maximum value should not be greater than 1500 minus kUDPIPHeaderBytes.
	 */
	static void SetPacketMTU(const int bytes = 1306);

	/**
	 * Allows the setting for SetPacketMTU() to be read.
	 * \return The setting for SetPacketMTU()
	 */
	static int GetPacketMTU(void);

	/**
	 * Sets the external IP address of this machine for machines that have a LAN IP and also external IP so that the UDP Transport will report the external IP instead of the internal IP.
	 * \param address The text IP address to set, for example "1.2.3.4". The default is "" meaning that the machine's real IP address will be reported.
	 */
	static void SetExternalIP(const std::string address = "");

	/**
	 * Allows the setting for SetExternalIP() to be read.
	 * \return The setting for SetExternalIP()
	 */
	static std::string GetExternalIP(void);

	/**
	 * If the info string is set any UDP or NATUDP listen connection will respond with a server info packet if a "0xff0xff0xff0xffgetinfo" packet is received. The info string is limited to 512 characters.
	 * \param info The default is "" which will disable the server response.
	 */
	static void SetInfoString(const std::string info = "");

	/**
	 * Allows the setting for SetInfoString() to be read.
	 * \return The setting for SetInfoString()
	 */
	static std::string GetInfoString(void);

	/**
	 * Allows the packet security to be enabled or disabled. Versions of RNXPURL prior to 6000 (ReplicaNet 6200, RNLobby 0700) do not use this extra security. To allow later versions to connect to earlier versions use SetExtraPacketSecurity(false). All old clients will still be able to connect to new servers. Security will be disabled for those connections.
	 * \param enable The default is true which will enable the extra packet security.
	 */
	static void SetExtraPacketSecurity(const bool enable = true);

	/**
	 * Allows the setting for SetExtraPacketSecurity() to be read.
	 * \return The setting for SetExtraPacketSecurity()
	 */
	static bool GetExtraPacketSecurity(void);

	/**
	 * Enables or disables detection of IPv4 capability over the internet.
	 * By default IPv4 detection is enabled.
	 */
	static void SetIPv4Detection(const bool enable = true);
	static bool GetIPv4Detection(void);

	/**
	 * Enables or disables detection of IPv6 capability over the internet.
	 * By default IPv6 detection is enabled.
	 */
	static void SetIPv6Detection(const bool enable = false);
	static bool GetIPv6Detection(void);

	/**
	 * Sets if IPv4 capability over the internet has been detected.
	 */
	static void SetIPv4Detected(const bool enable = true);
	static bool GetIPv4Detected(void);

	/**
	 * Sets if IPv6 capability over the internet has been detected.
	 */
	static void SetIPv6Detected(const bool enable = false);
	static bool GetIPv6Detected(void);

	/**
	 * Returns the numbers of bytes saved.
	 * \param reset If true then the counter is reset
	 */
	static size_t GetTransparentPacketCompressionBytesSaved(const bool reset = false);

	/**
	 * Allows the packet select code to be configured. If enabled with true then the packet update loop will be optimised to use packet selects, which will wait for packet data to arrive.
	 * Only enable this if the XPURL::Poll() method is set to automatuic poll and is never called from outside the XPURL update thread.
	 * \param enable The default is false which will disable the packet select code.
	 */
	static void SetEnablePacketSelect(const bool enable = false);

	/**
	 * Allows the setting for SetEnablePacketSelect() to be read.
	 * \return The setting for SetEnablePacketSelect()
	 */
	static bool GetEnablePacketSelect(void);

private:
	TransportConfig() {};
};

} // namespace RNReplicaNet

#endif
