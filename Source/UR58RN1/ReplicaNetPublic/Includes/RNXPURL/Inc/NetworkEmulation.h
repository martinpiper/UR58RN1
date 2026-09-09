/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __NETWORKEMULATION_H__
#define __NETWORKEMULATION_H__

namespace RNReplicaNet
{

/**
 * NetworkEmulation allows various emulated network settings to be changed to allow the user to test their application
 * with emulated packet loss, latency, jitter and bandwidth controls.<br>
 * This class is a static access class.
 */
class NetworkEmulation
{
public:

	/**
	 * Allows the network emulation to be turned on or off
	 * \param enable set this to be true or false to turn on or off the network emulation. The default is false.
	 */
	static void SetEnabled(const bool enable = false);

	/**
	 * Allows the network emulation activation status to be read
	 * \return true is the network emulation is enabled
	 */
	static bool GetEnabled(void);

	/**
	 * Enumeration for the SetConnection function for standard types of connection
	 */
	enum ConnectionTypes
	{
		kPerfect = 0,			/**< No packet loss, latency or bandwidth impairment.*/
		kModem28k,				/**< Simulates a 28k analogue modem*/
		kModem56k,				/**< Simulates a 56k analogue modem */
		kDSL512MB_Down128Up,	/**< Simulates a common type of DSL connection */
		kLAN					/**< Simulates a LAN network */
	};

	/**
	 * Allows the network emulation parameters to be set quickly for various standard testing types
	 * \param type the type of connection to emulate
	 * \param enable enable or disable the emulation
	 */
	static void SetConnection(const ConnectionTypes type = kPerfect,const bool enable = true);

	/**
	 * This sets the emulated packet loss for the network layer.
	 * \param percent Sets the emulated packet loss level where percent is any value from 0 to 100 percent packet loss. The default value is 0 meaning no packet loss.
	 */
	static void SetPacketLoss(const float percent = 0);

	/**
	 * This allows the configured emulated packet loss to be read.
	 * \return the emulated packet loss value
	 */
	static float GetPacketLoss(void);

	/**
	 * Sets the maximum output bandwidth for emulation.
	 * \param bytesPerSecond the bandwidth in bytes per second. The default value is 0 meaning there is no limit.
	 */
	static void SetMaximumOutputBandwidth(const int bytesPerSecond = 0);

	/**
	 * Reads the configuration value set by SetMaximumOutputBandwidth()
	 * \return the configured value
	 */
	static int GetMaximumOutputBandwidth(void);

	/**
	 * Sets the maximum input bandwidth for emulation.
	 * \param bytesPerSecond the bandwidth in bytes per second. The default value is 0 meaning there is no limit.
	 */
	static void SetMaximumInputBandwidth(const int bytesPerSecond = 0);

	/**
	 * Reads the configuration value set by GetMaximumInputBandwidth().
	 * \return the configured value
	 */
	static int GetMaximumInputBandwidth(void);

	/**
	 * Sets the average packet latency, this is half round trip time.
	 * \param milliSeconds the latency in seconds. So setting this to be 0.1f will mean a packet latency of 100ms is used. The default value is 0 meaning there is no latency.
	 */
	static void SetAverageLatency(const float seconds = 0.0f);

	/**
	 * Reads the configuration value set by SetAverageLatency()
	 * \return the configured value
	 */
	static float GetAverageLatency(void);

	/**
	 * Allows the amount by which the latency value can fluctuate
	 * \param milliSeconds the jitter in seconds. So setting this to be 0.05f and latency to 0.1f will mean a packet latency of 50ms to 150ms (100ms +-50ms) can be used. The default value is 0 meaning there is no jitter.
	 */
	static void SetJitter(const float seconds = 0.0f);

	/**
	 * Reads the configuration value set by SetJitter()
	 * \return the configured value
	 */
	static float GetJitter(void);

private:
	NetworkEmulation() {};
	virtual ~NetworkEmulation() {};
};

} // namespace RNReplicaNet

#endif
