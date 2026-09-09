/* START_LICENSE_HEADER

Copyright (C) 2000 Martin Piper, original design and program code
Copyright (C) 2001 Replica Software

This program file is copyright (C) Replica Software and can only be used under license.
For more information visit: http://www.replicanet.com/
Or email: info@replicanet.com

END_LICENSE_HEADER */
#include "RNPlatform/Inc/MemoryTracking.h"
#ifndef __REGISTRYMANAGERMAP_H__
#define __REGISTRYMANAGERMAP_H__

#include <assert.h>

#include <map>

namespace RNReplicaNet
{

/**
 * RegistryManager is a template class for handling classes in organised maps
 */
template <class I,class T,class C=std::less<I> > class RegistryManagerMap
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
	RegistryManagerMap()
	{
		mDirection = kUndefined;
		st = mItems.end();
	}

	/**
	 * The dtor makes sure everything is tidy
	 */
	virtual ~RegistryManagerMap()
	{
	}

	/**
	 * Adds an item to the list. This does not allocate an item as this is left to the user to do
	 * \param item the item pointer to add
	 */
	bool AddItem(const I &index,const T *item)
	{
		typedef typename std::map<I,T*,C>::iterator theIter;
		typename std::pair<theIter,bool> aMapResult;
		aMapResult = mItems.insert(std::pair<I,T*>(index,(T*)item));
		return aMapResult.second;
	}

	/**
	 * Finds an item using the index and if found also sets the current iterator to the item in the map.
	 * The direction of the iterator is set to be kUndefined.
	 * \param index The index to use.
	 * \return The pointer to T corresponding to the index or null if the index was not found.
	 */
	T* FindItem(const I &index)
	{
		typename std::map<I,T*,C>::iterator found;

		found = mItems.find(index);

		if (found != mItems.end())
		{
			st = found;
			mDirection = kUndefined;
			return (*found).second;
		}
		return 0;
	}

	/**
	 * Finds an item using the index. The current iterator is not set.
	 * \param index The index to use.
	 * \return The pointer to T corresponding to the index or null if the index was not found.
	 */
	T* FindItemNoIterator(const I &index)
	{
		typename std::map<I,T*,C>::iterator found;

		found = mItems.find(index);

		if (found != mItems.end())
		{
			return (*found).second;
		}
		return 0;
	}

	/**
	 * Removes an item from the list but does not free the item as this is left to the user to do
	 * \param item the item pointer to remove
	 * \return True indicates the index was in the map. False indicates the index was not in the map.
	 */
	bool RemoveItem(const I &index)
	{
		typename std::map<I,T*,C>::iterator found;

		found = mItems.find(index);

		if (found != mItems.end())
		{
			if (mDirection != kUndefined)
			{
				if (found == st)
				{
					if (mDirection == kForward)
					{
						st++;
						mItems.erase(found);
						return true;
					}

					if (mDirection == kBackward)
					{
						st--;
						mItems.erase(found);
						st++;
						return true;
					}
				}
			}
			mItems.erase(found);
			return true;
		}
		return false;
	}

	/**
	 * Removes the item at the current iterator.
	 */
	void RemoveItem(void)
	{
		switch(mDirection)
		{
			default:
			{
				assert(false && "Undefined direction state");
				break;
			}

			case kUndefined:
			{
				// No direction so it was probably a find so remove the iterator where it is at.
				mItems.erase(st);
				st = mItems.end();
				break;
			}

			case kForward:
			{
				typename std::map<I,T*,C>::iterator tst;
				st--;
				tst = st;
				st++;
				mItems.erase(tst);
				break;
			}

			case kBackward:
			{
				typename std::map<I,T*,C>::iterator tst;
				tst = st;
				st++;
				mItems.erase(tst);
				break;
			}
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
	 * \param index Allows the value of the index to be read. The default is null indicating the index is not returned.
	 * \return returns the next item pointer in the list or returns NULL for the end of the list
	 */
	T *Iterate(I *index = 0)
	{
		assert(mDirection != kUndefined && "RegistryManagerList::Iterate() used when BeginIterate() or EndIterate() not used or Iterate() reached the end of the list");

		if (mDirection == kForward)
		{
			if (st != mItems.end())
			{
				T *tclass = (*st).second;
				if (index)
				{
					*index = (*st).first;
				}
				st++;
				return tclass;
			}
		}

		if (mDirection == kBackward)
		{
			if (st != mItems.begin())
			{
				st--;
				T *tclass = (*st).second;
				if (index)
				{
					*index = (*st).first;
				}
				return tclass;
			}
		}

#ifdef _DEBUG
		mDirection = kUndefined;
#endif
		return 0;
	}

	/**
	 * Returns true if the map is empty
	 */
	bool IsEmpty(void)
	{
		return mItems.empty();
	}

	bool IsEmpty(void) const
	{
		return mItems.empty();
	}

	/**
	 * Clears the associated map. Does not free the items.
	 */
	void Clear(void)
	{
		mItems.clear();
		st = mItems.end();
	}

private:

	std::map<I,T*,C> mItems;					/**< The items list */
	typename std::map<I,T*,C>::iterator st;		/**< Start list iterator */
	Direction mDirection;
};

} // namespace RNReplicaNet

#endif
