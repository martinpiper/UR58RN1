/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef _RN_STRINGUTILS_H_
#define _RN_STRINGUTILS_H_
#include <string>

namespace RNReplicaNet
{

/// Trims leading and trailing whitespace. The whitespace characters can be changed to any other list of characters.
extern std::string TrimWhite(const std::string &t);

/// Produces a lower case string.
extern std::string ToLower( const std::string &input );

/// Converts a string to either int, float, double.
/// \param input The input string
/// \param defaultValue The default value if the input does not equal a suitable numeric string
/// \return The output value
template <typename T> extern T StringTo(const std::string &input , const T defaultValue = 0);

/// Converts an int, float or double to a string.
/// \param input The input value.
/// \param decimalPlaces The optional number of decimal places to output, -1 uses whatever default exists in the C library being used.
/// \return The output string
template <typename T> extern std::string ToString(const T input , const int decimalPlaces = -1 );

/// Validate characters for the account user name
/// Characters allowed are A-Z uppercase and lowercase, 0-9 and " !£$^&*():;@/.,-=`#~{}[]_".
/// Maximum length is 32 characters (UserServerPrivate::kMaximumUsernameSize) after white space trimming
extern std::string ValidateUsername(const std::string &input);

/// Validate characters for the account email address
/// Maximum length is 256 characters (UserServerPrivate::kMaximumEmailSize) after white space trimming
extern std::string ValidateEmail(const std::string &input);

/// Safely gets a string from memory up to the maxLen bytes. If the memory contains null the string can be shorter than maxLen.
extern std::string GetStringSafe( const char *input , const int maxLen );

extern std::string ReadTextFile( const char *filename );

} // namespace RNReplicaNet

#endif //#ifndef _RN_STRINGUTILS_H_
