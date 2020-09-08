#ifndef FREELIST_H
#define FREELIST_H

// TODOS,

// TODO, if no suitable memory was found, try defragmenting.
// TODO, if no suitable memory was found after defrag, ask memorymanager for more.
// TODO, while freeing memory, if neighbour was free and merge occurs then: Clear the added memory. Do a memcpy of zeros?

struct Entry
{
	Entry* pNext;
	size_t Size;
	void* pPayload;
	bool busy;
};

class FreeList
{
public:

	static void* Allocate(size_t size);
	template<typename T, typename ...Args>
	static T* Allocate(const Args& ... args);
	static void Free(void* ptr);
	template<typename T>
	static void Free(T* ptr);

private:
	FreeList();
	bool deFragList();
	Entry* findSuitableEntry(size_t size);
	static FreeList& getInstance();

	void* m_pMem;
	Entry* m_pHead;
};

#endif

template<typename T, typename ...Args>
inline T* FreeList::Allocate(const Args& ...args)
{
	T* ptr = new (FreeList::Allocate(sizeof(T))) T(args...);
	return ptr;
}

template<typename T>
inline void FreeList::Free(T* ptr)
{
	if (ptr)
	{
		ptr->~T();
		FreeList::Free(static_cast<void*>(ptr));
	}
}
