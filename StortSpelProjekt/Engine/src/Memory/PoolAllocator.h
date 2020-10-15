#ifndef POOLALLOCATOR_H
#define POOLALLOCATOR_H

template <typename T>
class PoolAllocator
{
public:
	static PoolAllocator<T>& GetInstance(unsigned int nrToReserve = 10);
	template<typename... Args>
	T* Allocate(Args... args);
	void Free(T* ptr);
	void Delete(T* ptr);
	void Reserve(unsigned int nrOf);
	~PoolAllocator();
	unsigned int Allocated() const;
	unsigned int Reserved() const;
private:

	struct PoolEntry
	{
		PoolEntry* next;
	};

	struct MemBlock
	{
		MemBlock* next;
	};

	PoolAllocator(unsigned int nrToReserve);

	// Nr of allocations given.
	unsigned int m_Allocated;
	// Nr of allocation held.
	unsigned int m_Reserved;

	PoolEntry* m_pFreeHead;
	MemBlock* m_pFirstBlock;
};

template<typename T>
inline PoolAllocator<T>& PoolAllocator<T>::GetInstance(unsigned int nrToReserve)
{
	static PoolAllocator<T> instance(nrToReserve);
	return instance;
}

template<typename T>
inline void PoolAllocator<T>::Free(T* ptr)
{
	PoolEntry* head = reinterpret_cast<PoolEntry*>(reinterpret_cast<char*>(ptr) - 8);
	head->next = m_pFreeHead;
	m_pFreeHead = head;
	m_Allocated--;
}

template<typename T>
inline void PoolAllocator<T>::Delete(T* ptr)
{
	ptr->~T();
	Free(ptr);
}

template<typename T>
inline void PoolAllocator<T>::Reserve(unsigned int nrOf)
{
	// Calculate how much memory we need!
	// One poolEntry and a object per allocations. also make room for one memblock object.
	unsigned int toAlloc = (8 + sizeof(T)) * nrOf + 8;

	// Aquire memory
	char* mem = reinterpret_cast<char*>(malloc(toAlloc));

	// Format the memory by placement new.

	MemBlock* nextFirst = new (mem) MemBlock;
	nextFirst->next = m_pFirstBlock;
	m_pFirstBlock = nextFirst;

	mem += 8;
	PoolEntry* last = m_pFreeHead;
	PoolEntry* entry;
	for (int i = 0; i < nrOf; i++)
	{
		entry = new (mem) PoolEntry;
		entry->next = last;
		last = entry;
		mem += 8 + sizeof(T);
	}
	m_pFreeHead = entry;
	m_Reserved += nrOf;
}

template<typename T>
inline PoolAllocator<T>::~PoolAllocator()
{
	MemBlock* next = nullptr;
	while (m_pFirstBlock)
	{
		next = m_pFirstBlock->next;
		free(m_pFirstBlock);
		m_pFirstBlock = next;
	}
}

template<typename T>
inline unsigned int PoolAllocator<T>::Allocated() const
{
	return m_Allocated;
}

template<typename T>
inline unsigned int PoolAllocator<T>::Reserved() const
{
	return m_Reserved;
}

template<typename T>
inline PoolAllocator<T>::PoolAllocator(unsigned int nrToReserve) :
	m_Allocated(0),
	m_Reserved(nrToReserve),
	m_pFirstBlock(nullptr),
	m_pFreeHead(nullptr)
{
	// Calculate how much memory we need!
	// One poolEntry and a object per allocations. also make room for one memblock object.
	unsigned int toAlloc = (8 + sizeof(T)) * nrToReserve + 8;

	// Aquire memory
	char* mem = reinterpret_cast<char*>(malloc(toAlloc));

	// Format the memory by placement new.

	m_pFirstBlock = new (mem) MemBlock;
	m_pFirstBlock->next = nullptr;

	mem += 8;
	PoolEntry* last = nullptr;
	for (int i = 0; i < nrToReserve; i++)
	{
		m_pFreeHead = new (mem) PoolEntry;
		m_pFreeHead->next = last;
		last = m_pFreeHead;
		mem += 8 + sizeof(T);
	}
}

template<typename T>
template<typename... Args>
inline T* PoolAllocator<T>::Allocate(Args... args)
{
	if (!m_pFreeHead)
	{
		Reserve(m_Reserved * 2);
	}
	T* toReturn = new (reinterpret_cast<char*>(m_pFreeHead) + 8) T(args...);
	m_pFreeHead = m_pFreeHead->next;
	m_Allocated++;
	return toReturn;
}

#endif