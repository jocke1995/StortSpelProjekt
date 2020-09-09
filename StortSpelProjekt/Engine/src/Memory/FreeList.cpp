#include "stdafx.h"
#include "FreeList.h"
#include <new>
#include "MemoryManager.h"

FreeList::FreeList(): m_pMem(nullptr), m_pHead(nullptr)
{
	m_pMem = MemoryManager::AllocHeapBlock();

	// If the memory is smaller or equal to the size of an entry, there is no point in ursing it.
	if (MemoryManager::GetBlockSize() <= sizeof(Entry))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL,"MEMORY ERROR: FreeList did not recieve enough memory!");
		return;
	}

	m_pHead = (Entry*)new (m_pMem) Entry;
	m_pHead->Size = MemoryManager::GetBlockSize() - sizeof(Entry);
	m_pHead->pPayload = (void*)((char*)m_pMem + sizeof(Entry));
	m_pHead->busy = false;
	m_pHead->pNext = nullptr;
}

void* FreeList::Allocate(size_t size)
{
	void* payload = nullptr;
	FreeList& fl = FreeList::getInstance();
	Entry* candidate = fl.findSuitableEntry(size);
	Entry* excess = nullptr;

	if (candidate)
	{
		// If candidate was found, the excess memory should become a new entry.
		if (candidate->Size > size + sizeof(Entry))
		{
			excess = new ((char*)candidate->pPayload + size) Entry;
			excess->busy = false;

			excess->pNext = candidate->pNext;
			candidate->pNext = excess;

			excess->Size = candidate->Size - size - sizeof(Entry);
			candidate->Size = size;

			excess->pPayload = (char*)excess + sizeof(Entry);
		}
		candidate->busy = true;
		payload = candidate->pPayload;
	}
	else if (fl.deFragList())
	{
		candidate = fl.findSuitableEntry(size);
	}
	else
	{
		size_t sizeOfNewMem = Max(size + sizeof(Entry), MemoryManager::GetBlockSize());
		void* mem = MemoryManager::AllocHeap(sizeOfNewMem);

		if (mem)
		{
			candidate = new (mem) Entry;
			candidate->Size = sizeOfNewMem - sizeof(Entry);
			candidate->pPayload = static_cast<char*>(mem) + sizeof(Entry);
			candidate->pNext = fl.m_pHead;

			if (candidate->Size > size + sizeof(Entry))
			{
				excess = new ((char*)candidate->pPayload + size) Entry;
				excess->busy = false;

				excess->pNext = candidate->pNext;
				candidate->pNext = excess;

				excess->Size = candidate->Size - size - sizeof(Entry);
				candidate->Size = size;

				excess->pPayload = (char*)excess + sizeof(Entry);
			}
			candidate->busy = true;
			payload = candidate->pPayload;
		}
	}
	// TODO, if no suitable memory was found after defrag, ask memorymanager for more.

	return payload;
}

void FreeList::Free(void* ptr)
{
	if (ptr)
	{
		Entry* entry = (Entry*)((char*)ptr - sizeof(Entry));

		entry->busy = false;

		// Merge the neighbours if they are both free :D
		if (entry->pNext)
		{
			if (!entry->pNext->busy)
			{
				entry->Size += entry->pNext->Size + sizeof(Entry);
				entry->pNext = entry->pNext->pNext;
			}
		}
	}
}

bool FreeList::deFragList()
{
	Entry* curr = m_pHead;
	bool res = false;
	
	while (curr->pNext)
	{
		if (!(curr->busy || curr->pNext->busy) && 
			(curr + curr->Size + sizeof(Entry) == curr->pNext))
		{
			curr->Size += curr->pNext->Size + sizeof(Entry);
			
			// probably unneccessary.
			// memcpy(curr->pNext, 0, sizeof(Entry));

			curr->pNext = curr->pNext->pNext;
			res = true;
		}
		curr = curr->pNext;
	}

	return res;
}

Entry* FreeList::findSuitableEntry(size_t size)
{
	Entry* candidate = nullptr;
	Entry* current = m_pHead;

	// First fit
	while (candidate == nullptr && current != nullptr)
	{
		if (current->Size >= size && !current->busy)
		{
			candidate = current;
		}
		current = current->pNext;
	}

	// If there is no candidate by first fit, there is no Entry appropriate for the requested memory.
	if (candidate)
	{
		// Best fit search.
		while (current != nullptr)
		{
			if (current->Size >= size && (current->Size < candidate->Size) && !current->busy)
			{
				candidate = current;
			}
		}
	}
	return candidate;
}

FreeList& FreeList::getInstance()
{
	static FreeList instance;
	return instance;
}
