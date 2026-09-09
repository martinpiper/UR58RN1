/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __MESSAGEHELPER_H__
#define __MESSAGEHELPER_H__
#include <string>

namespace RNReplicaNet
{

#define MESSAGEHELPER_ADDVARIABLE(x)	\
AddVariable(&(x),sizeof(x));

#define MESSAGEHELPER_GETVARIABLE(x)	\
GetVariable(&(x),sizeof(x));

#define MESSAGEHELPER_ADDVARIABLEp(y,x)	\
y.AddVariable(&(x),sizeof(x));

#define MESSAGEHELPER_GETVARIABLEp(y,x)	\
y.GetVariable(&(x),sizeof(x));

/**
 * A simple message helper class that helps to construct and deconstruct sequences of variables.
 * This routine is endian safe and should be used where data needs to be endian safe
 */
class MessageHelper
{
public:
	class Exception
	{
	public:
		Exception() {}
		virtual ~Exception() {}
	};
	/**
	 * The ctor performs some basic initialisation
	 */
	MessageHelper();

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~MessageHelper();

	/**
	 * Sets the data buffer pointer to use while constructing or deconstructing this message. This also causes the length of the buffer to revert to zero.
	 * \param buffer the buffer pointer to set
	 */
	void SetBuffer(void *const buffer);

	/**
	 * Gets the pointer to the start of the buffer.
	 * \return The pointer to the start of the buffer.
	 */
	void *GetBuffer(void) const;

	/**
	 * Gets the pointer to the current position in the buffer.
	 * \return The pointer to the current position in the buffer.
	 */
	void *GetCurrentPosition(void) const;

	/**
	 * Adds a variable of a specified size to the message buffer while using an endian check to preserve the value between platforms with different endian formats
	 * \param variable the pointer to the variable
	 * \param size the size of the data
	 */
	virtual void AddVariable(const void *variable,const int size);
	/// Adds data to a buffer with endian checking
	static void AddVariable(void *buffer, const void *variable, const int size);

	/**
	 * Adds a data block of a specified size to the message buffer without using any endian checking
	 * \param data the pointer to the data
	 * \param size the size of the data
	 */
	virtual void AddData(const void *data,const int size);

	/**
	 * Gets a variable of a specified size from the message buffer
	 * \param variable the pointer to the variable
	 * \param size the size of the data
	 */
	void GetVariable(void *const variable,const int size);
	/// Adds data from a buffer with endian checking
	static void GetVariable(const void *buffer, void *const variable, const int size);

	/**
	 * Gets data of a specified size from the message buffer
	 * \param data the pointer to the data
	 * \param size the size of the data
	 */
	void GetData(void *const data,const int size);

	/**
	 * Skips data of a specified size from the message buffer
	 * \param size the size of the data
	 */
	void SkipData(const int size);

	/**
	 * Returns the current size of the message in bytes
	 * \return returns the size of the message buffer in bytes
	 */
	int GetSize(void) const;

	/**
	 * Sets the current size of the message in bytes.
	 * \param the size of the message in bytes
	 */
	void SetSize(const int size);

	/**
	 * Adds an integer to the message buffer
	 * \param value the value to add
	 */
	void AddInteger(const int value);

	/**
	 * Adds a float to the message buffer
	 * \param value the value to add
	 */
	void AddFloat(const float value);

	/**
	 * Gets an integer from the message buffer
	 * \param value the value to get
	 */
	int GetInteger(void);

	/**
	 * 
	 * Gets a float from the message buffer
	 * \param value the value to get
	 */
	float GetFloat(void);

	// Direct buffer modification functions with endian checking
	// The offset is in chars into the buffer.
	static int GetInt(const void *buffer, const int offset);
	static short GetShort(const void *buffer, const int offset);
	static void SetInt(void *buffer, const int offset, const int value);
	static void SetShort(void *buffer, const int offset, const short value);

	/**
	 * This produces a hex dump of any length of data
	 */
	static std::string DumpAsHex(const void *data,const int size,const bool csv = true);

	std::string DumpAsHex(const bool csv = true) const
	{
		return DumpAsHex(GetBuffer(),GetSize(),csv);
	}

	/**
	 * Sets the guard size and if the MessageHepler reads past the guard a MessageHepler::Exception() exception is thrown.
	 * \param size If < 0 then the exception is disabled. Otherwise an exception is thrown when data is read beyond the size configured.
	 */
	void SetGuardSize(const int size = -1);
	void SanityCheckGuardSize(const int newSize = 0);

	/**
	 * Returns the last size added to this MessageHelper.
	 */
	int GetLastSizeAdded(void) const;

	// Define operator<< and operator>> to make using this class a little easier.
	// For all operator>> the rhs is replaced with the data in lhs.
	// Generic cases
	template <typename T>
	void operator<<(const T& rhs)
	{
		MESSAGEHELPER_ADDVARIABLE(rhs);
	}
	template <typename T>
	void operator>>(T& rhs)
	{
		MESSAGEHELPER_GETVARIABLE(rhs);
	}
	
	// Special cases
	void operator<<(const char *rhs);
	void operator<<(const std::string &rhs);
	void operator>>(std::string &rhs);

	// For platforms where sizeof(bool) is different
	void operator<<(const bool &rhs);
	void operator>>(bool &rhs);

private:
	unsigned char *mFirstPtr;			/**< The data buffer pointer */
	unsigned char *mBufferPtr;			/**< Backup of the pointer passed in used to calculate size */
	int mGuardSize;

protected:
	int mLastSizeAdded;
};

/**
 * This extends the interface provided by MessageHelper to include a data buffer that dynamically grows to accommodate new data.
 */
class DynamicMessageHelper : public MessageHelper
{
public:
	/**
	 * The ctor performs some basic initialisation
	 */
	DynamicMessageHelper();

	/**
	 * The ctor performs some basic initialisation.
	 * \param size If size is greater than 0 then the buffer is allocated to be that size.
	 */
	DynamicMessageHelper( const int size );

	/**
	 * The ctor performs some basic initialisation.
	 * \param message If message is not null then the data is copied to the message for size bytes. If message is null then a buffer of size is allocated but there is no copy of data.
	 * \param size If size is greater than 0 then the buffer is allocated to be that size.
	 */
	DynamicMessageHelper( const void * message , const int size );

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~DynamicMessageHelper();

private:
	/**
	 * This function is private to generate a compile error if used. SetSize(0) should be used instead of SetBuffer(pointer)
	 */
	void SetBuffer(void *const) {}
public:

	/**
	 * Sets the data buffer size to use while constructing or deconstructing this message. Any previous buffer is freed.
	 * The buffer is not allocated immediately, buffer allocation happens when data is added to the buffer. To force an allocation of the buffer use EnsureBufferAllocated().
	 * \param buffer the buffer pointer to set. The default size is 1024 bytes.
	 */
	void SetBufferSize(const int size = 1024);

	/**
	 * Gets the value configured by SetBufferSize().
	 * \return The value configured by SetBufferSize().
	 */
	int GetBufferSize(void);

	/**
	 * Sets the current size of the message in bytes, allocating a buffer if required.
	 * \param the size of the message in bytes
	 */
	void SetSize(const int size);


	void AddVariable(const void *variable,const int size);

	void AddData(const void *data,const int size);

	/**
	 * This frees the buffer used by this class and sets the current size to 0. This also calls SetBufferSize() to reset the size to the default value.
	 */
	void FreeBuffer(void);

	/**
	 * This ensures the buffer is allocated.
	 */
	void EnsureBufferAllocated(void);

	/**
	 * Writes the contents of this class to a file.
	 * \param rawData If true this will write the raw data of the file and not add the DynamicMessageHelper file header.
	 */
	bool Write(const char *filename,const bool rawData = false);

	/**
	 * Reads the contents of a file to this class. GetBufferSize() will return the size of the message in the file and GetSize() will return 0.
	 * \param rawData If true this will read the raw data of the file and not check for the DynamicMessageHelper file header.
	 */
	bool Read(const char *filename,const bool rawData = false);

	// Define operator<< and operator>> to make using this class a little easier.
	// For all operator>> the rhs is replaced with the data in lhs.
	// Generic cases
	template <typename T>
	void operator<<(const T& rhs)
	{
		MESSAGEHELPER_ADDVARIABLE(rhs);
	}
	template <typename T>
	void operator>>(T& rhs)
	{
		MESSAGEHELPER_GETVARIABLE(rhs);
	}

	// Special cases
	void operator<<(const char *rhs);
	void operator<<(const std::string &rhs);
	void operator>>(std::string &rhs);

	void operator<<(const DynamicMessageHelper &rhs);
	void operator>>(DynamicMessageHelper &rhs);

	// Tests using GetSize() for the length of the data to test.
	bool operator==(const DynamicMessageHelper &rhs) const;
	bool operator!=(const DynamicMessageHelper &rhs) const;

	// For platforms where sizeof(bool) is different
	void operator<<(const bool &rhs);
	void operator>>(bool &rhs);

	/// Reads a hex encoded buffer without commas and converts it to binary data from the current position of the buffer.
	/// If there is an error reading the data then the result of the buffer is undefined.
	/// \return Success returns true, failure returns false.
	bool ReadAsHex(const std::string &data);

private:
	void CheckBufferSize(const int newSize);
	void *mBuffer;
	int mSize;
};

} // namespace RNReplicaNet

#endif
