/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#ifndef _RNPLATFORMCONFIG_H_
#define _RNPLATFORMCONFIG_H_

// This pragma disables a harmless warning in the older Microsoft compiler. This can be safely ignored. See bug ref: C4786
#ifdef _MSC_VER
#pragma warning (disable : 4786)
#endif

// Preprocessor defines for supported platforms:
// _WIN32			Windows 32/64 bit builds
// __MINGW32__		MinGW for Windows
// __linux__		Linux
// __unix__			Unix
// _XBOX			Microsoft Xbox 360, also has _WIN32 defined.
// _PS2				Sony PlayStation 2
// __APPLE__		Apple products using Xcode. eg. iOS, iPhone, iPad etc
// __ANDROID__		Android NDK
// IW_SDK			Marmalade SDK. Multiple platforms.

// Unix/Linux like platforms can be grouped together into one define for most cases
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__) || defined(__ANDROID__) || defined(IW_SDK)
#define RN_UNIX_LIKE
#if defined(__PROSPERO__) || defined(__ORBIS__)
#define RN_PS4_PS5
#endif
#endif

// Define RN_EXCEPTIONS_IGNORE to ignore this check
#if !defined(RN_EXCEPTIONS_IGNORE)
// Platforms that have exceptions enabled by default
#if defined(_WIN32) || defined(__MINGW32__) || defined(__linux__) || defined(__unix__) || defined(__APPLE__) || defined(__ANDROID__)
#define RN_EXCEPTIONS_ENABLED
#endif
#endif

#endif
