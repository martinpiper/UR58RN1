/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __ENCRYPTION_H__
#define __ENCRYPTION_H__

namespace RNReplicaNet
{

// Key length must be an integer multiple of 4 and be at least 8
const int kEncryptionKeyLengthBytes = 8;

/**
 * An symmetric encryption class that is designed to be fast and maintain the size of the original data.
 */
class Encryption
{
public:
	struct Key
	{
		public:
		Key();
		Key(const Key &source);
		virtual ~Key();

		/**
		 * Creates a key from data and an input length.
		 * \param data the data to use as a seed for the key. This can be NULL to use the default key.
		 * \param length the length of the data to use for creating the key
		 */
		void Create(const void *data,const int length);

		/**
		 * Adds the data to an existing crypto key, for example a salt value to cause the key to change for each block of data.
		 * \param data the data to use as a seed for the key. This can be NULL to use the default key.
		 * \param length the length of the data to use for creating the key
		 */
		void AddCrypto(const void *data,const int length);

		void AddCrypto(const unsigned char salt);

		unsigned char mKey[kEncryptionKeyLengthBytes];
	};

	/**
	 * Encrypt a portion of memory with a key.
	 * \param data the pointer to the start of the memory
	 * \param length the length of the memory in bytes
	 * \param key the key to use
	 */
	static void Encrypt(void *data,int length,Key *key);

	/**
	 * Decrypt a portion of memory with a key.
	 * \param data the pointer to the start of the memory
	 * \param length the length of the memory in bytes
	 * \param key the key to use
	 */
	static void Decrypt(void *data,int length,Key *key);


	/**
	 * Encrypt a portion of memory with a key with a commutative algorithm.
	 * \param data the pointer to the start of the memory
	 * \param length the length of the memory in bytes
	 * \param key the key to use
	 */
	static void CommutativeEncrypt(void *data,int length,Key *key);

	/**
	 * Decrypt a portion of memory with a key with a commutative algorithm.
	 * \param data the pointer to the start of the memory
	 * \param length the length of the memory in bytes
	 * \param key the key to use
	 */
	static void CommutativeDecrypt(void *data,int length,Key *key);

	/**
	 * Encrypt a portion of memory with a key with a commutative bytewise algorithm.
	 * \param data the pointer to the start of the memory
	 * \param length the length of the memory in bytes
	 * \param key the key to use
	 */
	static void CommutativeEncryptBytewise(void *data,int length,Key *key);

	/**
	 * Decrypt a portion of memory with a key with a commutative bytewise algorithm.
	 * \param data the pointer to the start of the memory
	 * \param length the length of the memory in bytes
	 * \param key the key to use
	 */
	static void CommutativeDecryptBytewise(void *data,int length,Key *key);
};

} // namespace RNReplicaNet

#endif
