/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __REGISTRYMANAGERLIST_H__
#define __REGISTRYMANAGERLIST_H__

#include <assert.h>

#include <list>

namespace RNReplicaNet
{

/**
 * RegistryManager is a template class for handling classes in organised linked lists
 */
template <class T> class RegistryManagerList
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
	RegistryManagerList()
	{
		mDirection = kUndefined;
	}

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~RegistryManagerList()
	{
	}

	/**
	 * Adds an item to the list. This does not allocate an item as this is left to the user to do
	 * \param item the item pointer to add
	 */
	void AddItem(T *item)
	{
		mItems.push_back(item);
	}

	/**
	 * Adds an item to the head of the list. This does not allocate an item as this is left to the user to do
	 * \param item the item pointer to add
	 */
	void AddItemHead(T *item)
	{
		mItems.push_front(item);
	}

	/**
	 * Removes an item from the list but does not free the item as this is left to the user to do
	 * \param item the item pointer to remove
	 */
	void RemoveItem(T *item)
	{
		typename std::list<T *>::iterator tst,ten;

		tst = mItems.begin();
		ten = mItems.end();

		while(tst != ten)
		{
			T *titem = *tst;
			if (titem == item)
			{
				if (mDirection != kUndefined)
				{
					if (tst == st)
					{
						if (mDirection == kForward)
						{
							st++;
							mItems.erase(tst);
							return;
						}

						if (mDirection == kBackward)
						{
							st--;
							mItems.erase(tst);
							st++;
							return;
						}
					}
				}
				mItems.erase(tst);
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
		assert(mDirection != kUndefined && "RegistryManagerList::RemoveItem() used when BeginIterate() or EndIterate() not used or Iterate() reached the ned of the list");

		typename std::list<T *>::iterator tst;

		if (mDirection == kForward)
		{
			st--;
			tst = st;
			st++;
			mItems.erase(tst);
			return;
		}

		if (mDirection == kBackward)
		{
			tst = st;
			st++;
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
		assert(mDirection != kUndefined && "RegistryManagerList::Iterate() used when BeginIterate() or EndIterate() not used or Iterate() reached the end of the list");

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

private:

	std::list<T *> mItems;			/**< The items list */
	typename std::list<T *>::iterator st;	/**< Start list iterator */
	Direction mDirection;
};

} // namespace RNReplicaNet

#endif
