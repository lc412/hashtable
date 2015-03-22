#ifndef _HASH_H_
#define _HASH_H_

#ifndef NULL
#define NULL (0L)
#endif

#define DEFAULT_TABLE_SIZE 128
#define TYPE			 (0x9C)		/* If You Change This, You Change The Hash Output! */

//
// this is my hash function
// it is built upon several months of research
// and should withstand VERY heavy testing without
// duplicate hashes
//
int toHash (const char* pString);

//
// my generic hash table implementation, simply pass it the
// class that you would like to be contained
//
template<class T>
class CHashTable
{
protected:
	template<class D>
	struct hEntry_s
	{
		T		     entry;
		int	     	 type;
		hEntry_s<D>* next;
	};

	int nCount;

	hEntry_s<T>** pTable;
	int 		  iSize;

public:
	CHashTable (int size = DEFAULT_TABLE_SIZE)
	{
		// allocate the table and zero it out
		iSize = size;
		pTable = new hEntry_s<T>*[size];

		for (int i = 0; i < iSize; i++)
		{
			pTable[i] = NULL;
		}

		nCount = 0;
	}

	~CHashTable ()
	{
		// free all our memory resources
		hEntry_s<T> *pEnt, *pTmp;

		for (int i = 0; i < iSize; i++)
		{
			if (pTable[i])
			{
				pEnt = pTable[i];

				while (pEnt)
				{
					pTmp = pEnt;
					pEnt = pEnt->next;
					delete pTmp;
				}

				pTable[i] = NULL;
			}
		}
	}

	int  getCount () { return nCount; }

	//
	// this adds an entry to a position in the hash table
	// I would typically 'hash' the name of the object
	// being inserted
	//
	bool addHash (int nHash, T entry)
	{
		int nPos, nType;

		// we use the top 16 bits as a collision detection tool
		// and the bottom 16 bits as an index
		nPos = (nHash & 0xFFFF) % iSize;
		nType = nHash >> 16;

		hEntry_s<T> *pEnt;

		pEnt = pTable[nPos];

		// if theres already an element here, append a new node to the linked list
		if (pEnt)
		{
			while (pEnt->next != NULL)
				pEnt = pEnt->next;

			pEnt->next = new hEntry_s<T>;

			pEnt = pEnt->next;
		}
		else
		{
			// otherwise, start a new linked list
			pTable[nPos] = pEnt = new hEntry_s<T>;
		}

		pEnt->entry = entry;
		pEnt->type  = nType;
		pEnt->next  = NULL;

		nCount++;

		return true;
	}

	//
	// this is if you retrieve the entry (via getHash ())
	// and make some changes to it, this will update it
	//
	bool updateHash (int nHash, T* entry)
	{
		int nPos, nType;

		// we use the top 16 bits as a collision detection tool
		// and the bottom 16 bits as an index
		nPos = (nHash & 0xFFFF) % iSize;
		nType = nHash >> 16;

		hEntry_s<T> *pEnt;

		pEnt = pTable[nPos];

		if (!pEnt)
			return false;

		// loop through all items in this indices linked list
		while (pEnt->type != nType)
		{
			if (pEnt->next != NULL)
				pEnt = pEnt->next;
			else
				break;
		}

		// check for our verification 16 bits
		if (pEnt->type == nType)
		{
			pEnt->entry = *entry;
			return true;
		}

		return false;
	}

	//
	// this will return the entry with the
	// given hash
	//
	bool getHash (int nHash, T* entry)
	{
		int nPos, nType;

		// we use the top 16 bits as a collision detection tool
		// and the bottom 16 bits as an index
		nPos = (nHash & 0xFFFF) % iSize;
		nType = nHash >> 16;

		hEntry_s<T> *pEnt;

		pEnt = pTable[nPos];

		// is there even a node here?
		if (!pEnt)
			return false;

		// loop through all items in this indices linked list
		while (pEnt->type != nType)
		{
			if (pEnt->next != NULL)
				pEnt = pEnt->next;
			else
				break;
		}

		// check for our verification 16 bits
		if (pEnt->type == nType)
		{
			*entry = pEnt->entry;
			return true;
		}

		return false;
	}

	//
	// I use this when I use the table for pointers to memory
	// this will return the first entry in the table (not the
	// first slot, but the first actual entry) and it's paired
	// hash value
	//
	bool getFirst (int* nHash, T* entry)
	{
		// find the first element in the table
		for (int i = 0; i < iSize; i++)
			if (pTable[i])
			{
				*entry = pTable[i]->entry;
				*nHash = i | (pTable[i]->type << 16);
				return true;
			}

		return false;
	}

	//
	// this will return the next hash in the list, given
	// an original hash value (again, I use this for freeing memory)
	//
	// Watch Out: It searches for the previous entry to give the next entry.
	// If you erase the previously returned entry (removeHash ()) you must
	// use getHash () to return another entry, because getNext () won't find
	// anymore and return false
	//
	bool getNext (int nHash, int* nNext, T* entry)
	{
		int nPos, nType;

		// we use the top 16 bits as a collision detection tool
		// and the bottom 16 bits as an index
		nPos = (nHash & 0xFFFF) % iSize;
		nType = nHash >> 16;

		hEntry_s<T> *pEnt;

		pEnt = pTable[nPos];

		// is there even a node here?
		if (!pEnt)
			return false;

		// shortcut: take the next item in the linked list if possible
		while (pEnt->type != nType)
		{
			if (pEnt->next != NULL)
				pEnt = pEnt->next;
			else
				break;
		}

		// loop through the table, and find the passed in node
		if (pEnt->type == nType)
		{
			// try to take the next node in the linked list, if possible
			if (pEnt->next != NULL)
			{
				pEnt = pEnt->next;
				*entry = pEnt->entry;
				*nNext = nPos | (pEnt->type << 16);

				return true;
			}

			int i;

			// oh well, keep searching the table
			for (int d = 0; d < iSize; d++)
			{
				i = d + nPos + 1;

				if (i >= iSize)
					i -= iSize;

				if (pTable[i])
				{
					*entry = pTable[i]->entry;
					*nNext = i | (pTable[i]->type << 16);
					return true;
				}
			}
		}

		// we got no more nodes!
		return false;
	}

	//
	// this will remove a hash from the table, freeing
	// up the slot
	//
	bool removeHash (int nHash)
	{
		int nPos, nType;

		// we use the top 16 bits as a collision detection tool
		// and the bottom 16 bits as an index
		nPos = (nHash & 0xFFFF) % iSize;
		nType = nHash >> 16;

		hEntry_s<T> *pEnt, *pPrev;

		pPrev = pEnt = pTable[nPos];

		// is there even a node here?
		if (!pEnt)
			return false;

		// shortcut: is the root node the one we want?
		if (pEnt->type == nType)
		{
			pPrev = pEnt->next;
			delete pEnt;
			pTable[nPos] = pPrev;

			nCount--;

			return true;
		}

		pEnt = pEnt->next;

		// any nodes left at this index?
		if (!pEnt)
			return false;

		// keep searching through this linked list
		while (pEnt->type != nType)
		{
			if (pEnt->next != NULL)
			{
				pPrev = pEnt;
				pEnt = pEnt->next;
			}
			else
				break;
		}

		// did we find it?
		if (pEnt->type == nType)
		{
			// delete it
			pPrev->next = pEnt->next;
			delete pEnt;

			nCount--;

			return true;
		}

		// nothing...
		return false;
	}
};

#endif