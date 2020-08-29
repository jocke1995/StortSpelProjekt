#ifndef FREELIST_H
#define FREELIST_H
#include "Allocator.h"

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

class FreeList : public Allocator
{
private:
	void* m_pMem;
	Entry* m_pFreeHead;
public:
	FreeList(void* mem, size_t size);

	void* Allocate(size_t size);
	void Free(void* ptr);
};

#endif