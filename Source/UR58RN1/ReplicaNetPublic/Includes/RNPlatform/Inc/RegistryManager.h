/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __REGISTRYMANAGER_H__
#define __REGISTRYMANAGER_H__

#include <assert.h>

#include <vector>

namespace RNReplicaNet
{

/**
 * RegistryManager is a template class for handling classes in organised vector arrays
 */
template <class T> class RegistryManager
{
public:
	/**
	 * The ctor performs some basic initialisation
	 */
	RegistryManager()
	{
		mUsed = false;
	}

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~RegistryManager()
	{
	}

	/**
	 * Adds an item to the vector. This does not allocate an item as this is left to the user to do
	 * \param item the item pointer to add
	 */
	void AddItem(T *item)
	{
		mItems.push_back(item);
	}

	/**
	 * Removes an item from the vector but does not free the item as this is left to the user to do
	 * \param item the item pointer to remove
	 */
	void RemoveItem(T *item)
	{
		typename std::vector<T *>::iterator tst,ten;

		tst = mItems.begin();
		ten = mItems.end();

		while(tst != ten)
		{
			T *titem = *tst;
			if (titem == item)
			{
				int offset = 0;
				if (mUsed)
				{
					offset = st - mItems.begin();
					if (tst < st)
					{
						offset--;
					}
				}
				mItems.erase(tst);
				if (mUsed)
				{
					st = mItems.begin() + offset;
				}
				return;
			}
			tst++;
		}
	}

	/**
	 * Removes the current iterated item
	 */
	void RemoveItem(void)
	{
#ifdef _DEBUG
		assert(mUsed != false && "RegistryManager::RemoveItem() used when BeginIterate() not used or Iterate() reached the end of the list");
#endif

		int offset = st - mItems.begin();
		mItems.erase(st);
		st = mItems.begin() + offset;
	}

	/**
	 * Starts the iteration of the items in the list
	 */
	void BeginIterate(void)
	{
		mUsed = true;
		st = mItems.begin();
	}

	/**
	 * Iterates through the items in the list
	 * \return returns the next item pointer in the vector or returns NULL for the end of the list
	 */
	T *Iterate(void)
	{
#ifdef _DEBUG
		assert(mUsed != false && "RegistryManager::Iterate() used when BeginIterate() not used or Iterate() reached the end of the list");
#endif

		if (st != mItems.end())
		{
			T *tclass = *st;
			st++;
			return tclass;
		}

		mUsed = false;
		return 0;
	}

	bool IsEmpty(void) const
	{
		return mItems.empty();
	}

private:
	std::vector<T *> mItems;			/**< The items list */

	typename std::vector<T *>::iterator st,en;	/**< Start and end list iterators */

	bool mUsed;	// Extra STL iterator checking
};

} // namespace RNReplicaNet

#endif
