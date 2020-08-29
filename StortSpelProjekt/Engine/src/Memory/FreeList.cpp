#include "stdafx.h"
#include "FreeList.h"
#include <new>
FreeList::FreeList(void* mem, size_t size): m_pMem(nullptr), m_pFreeHead(nullptr)
{
	// If the memory is smaller or equal to the size of an entry, there is no point in ursing it.
	if (size <= sizeof(Entry))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL,"MEMORY ERROR: FreeList did not recieve enough memory!");
		return;
	}

	m_pMem = mem;
	m_pFreeHead = (Entry*)new (m_pMem) Entry;
	m_pFreeHead->Size = size - sizeof(Entry);
	m_pFreeHead->pPayload = (void*)((char*)m_pMem + sizeof(Entry));
	m_pFreeHead->busy = false;
}

void* FreeList::Allocate(size_t size)
{
	void* payload = nullptr;
	Entry* candidate = nullptr;
	Entry* current = m_pFreeHead;

	// First fit
	while (candidate == nullptr && current != nullptr)
	{
		if (current->Size > size && !current->busy)
		{
			candidate = current;
			payload = candidate->pPayload;
		}

		current = current->pNext;
	}

	// Best fit search.
	while (current != nullptr)
	{
		if (current->Size >= size && (current->Size < candidate->Size)&& !current->busy)
		{
			candidate = current;
			payload = candidate->pPayload;
		}
	}

	// If candidate was found, the excess memory should be prepared
	if (candidate)
	{
		if (candidate->Size > size)
		{
			current = new ((char*)candidate->pPayload + size) Entry;
			current->busy = false;
			
			current->pNext = candidate->pNext;
			candidate->pNext = current;
			
			current->Size = candidate->Size - size - sizeof(Entry);
			candidate->Size = size;
			
			current->pPayload = (char*)current + sizeof(Entry);
		}
	}
	candidate->busy = true;

	// TODO, if no suitable memory was found, try defragmenting.

	// TODO, if no suitable memory was found after defrag, ask memorymanager for more.

	return payload;
}

void FreeList::Free(void* ptr)
{
	Entry* entry = (Entry*)((char*)ptr - sizeof(Entry));

	entry->busy = false;

	// Merge the neighbours if they are both free :D
	if (!entry->pNext->busy)
	{
		entry->Size += entry->pNext->Size + sizeof(Entry);
		entry->pNext = entry->pNext->pNext;
		// TODO, Clear the memory. do a memcpy of zeros?
	}
}
