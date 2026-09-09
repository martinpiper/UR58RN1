/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __REGISTRYMANAGERSET_H__
#define __REGISTRYMANAGERSET_H__

#include <assert.h>

#include <set>

namespace RNReplicaNet
{

/**
 * RegistryManager is a template class for handling classes in a set
 */
template <class T,class C = std::less<T*> > class RegistryManagerSet
{
public:
	enum Direction
	{
		kForward=0,
		kBackward,
		kUndefined
	};

	/**
	 * The ctor performs some basic initialisation
	 */
	RegistryManagerSet()
	{
		mDirection = kUndefined;
		mNumItems = 0;
	}

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~RegistryManagerSet()
	{
	}

	/**
	 * Adds an item to the list. This does not allocate an item as this is left to the user to do
	 * \param item the item pointer to add
	 */
	void AddItem(T *item)
	{
		mItems.insert(item);
		mNumItems++;
	}

	/**
	 * Removes an item from the list but does not free the item as this is left to the user to do
	 * \param item the item pointer to remove
	 */
	void RemoveItem(T *item)
	{
		assert(mNumItems && "RegistryManagerSet::RemoveItem(T *item) called when there were no items");
		typename std::set<T *,C>::iterator found = mItems.find(item);

		if (found != mItems.end())
		{
			mNumItems--;
			if (mDirection != kUndefined)
			{
				if (found == st)
				{
					if (mDirection == kForward)
					{
						st++;
						mItems.erase(found);
						return;
					}

					if (mDirection == kBackward)
					{
						st--;
						mItems.erase(found);
						st++;
						return;
					}
				}
			}
			mItems.erase(found);
			return;
		}
	}

	/**
	 * Removes the current iterated item
	 */
	void RemoveItem(void)
	{
		assert(mDirection != kUndefined && "RegistryManagerSet::RemoveItem() used when BeginIterate() or EndIterate() not used or Iterate() reached the end of the list");
		assert(mNumItems && "RegistryManagerSet::RemoveItem() called when there were no items");

		typename std::set<T *,C>::iterator tst;

		if (mDirection == kForward)
		{
			st--;
			tst = st;
			st++;
			mNumItems--;
			mItems.erase(tst);
			return;
		}

		if (mDirection == kBackward)
		{
			tst = st;
			st++;
			mNumItems--;
			mItems.erase(tst);
		}
	}

	/**
	 * Starts the iteration of the items in the list from the start and works forwards
	 */
	void BeginIterate(void)
	{
		mDirection = kForward;
		st = mItems.begin();
	}

	/**
	 * Starts the iteration of the items in the list from the end and works backwards
	 */
	void EndIterate(void)
	{
		mDirection = kBackward;
		st = mItems.end();
	}

	/**
	 * Iterates through the items in the list
	 * \return returns the next item pointer in the list or returns NULL for the end of the list
	 */
	T *Iterate(void)
	{
		assert(mDirection != kUndefined && "RegistryManagerSet::Iterate() used when BeginIterate() or EndIterate() not used or Iterate() reached the ned of the list");

		if (mDirection == kForward)
		{
			if (st != mItems.end())
			{
				T *tclass = *st;
				st++;
				return tclass;
			}
		}

		if (mDirection == kBackward)
		{
			if (st != mItems.begin())
			{
				st--;
				T *tclass = *st;
				return tclass;
			}
		}

#ifdef _DEBUG
		mDirection = kUndefined;
#endif
		return 0;
	}

	bool IsEmpty(void) const
	{
		return mItems.empty();
	}

	size_t GetNumItems(void) const
	{
		return mNumItems;
	}

private:

	std::set<T *,C> mItems;			/**< The items list */
	typename std::set<T *,C>::iterator st;	/**< Start list iterator */
	Direction mDirection;
	size_t mNumItems;
};

} // namespace RNReplicaNet

#endif
