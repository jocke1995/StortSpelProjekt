#ifndef STACKALLOCATOR_H
#define STACKALLOCATOR_H

class StackAllocator
{
public:
	StackAllocator(void* start, void* end);

	void* Allocate(size_t size);

	template<typename T, typename ...Args>
	T* Allocate(Args... args);

	void Free(void* ptr);

	template<typename T>
	void Free(T* ptr);

private:
	char* m_pStart;
	char* m_pEnd;
	char* m_pCurrent;
};

#endif

template<typename T, typename ...Args>
inline T* StackAllocator::Allocate(Args ...args)
{
	//pekare till minnet = new (plats att lägga minne) objektstyp
	T* ptr = new (Allocate(sizeof(T))) T(args...);
	return ptr;
}

template<typename T>
inline void StackAllocator::Free(T* ptr)
{
	ptr->~T();
	Free(static_cast<void*>ptr);
}
