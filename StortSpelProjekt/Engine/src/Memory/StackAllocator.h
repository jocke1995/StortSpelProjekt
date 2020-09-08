#ifndef STACKALLOCATOR_H
#define STACKALLOCATOR_H

class StackAllocator
{
public:

	static void* Allocate(size_t size);

	template<typename T, typename ...Args>
	static T* Allocate(Args... args);

	static void Free(void* ptr);

	template<typename T>
	static void Free(T* ptr);

private:
	
	StackAllocator();
	static StackAllocator& getInstace();
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
	Free(static_cast<void*>(ptr));
}
