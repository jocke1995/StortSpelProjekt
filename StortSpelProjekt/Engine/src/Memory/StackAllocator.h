#ifndef STACKALLOCATOR_H
#define STACKALLOCATOR_H

class StackAllocator
{
public:
	StackAllocator(void* start, void* end);

	void* Allocate(size_t size);

	void Free(void* ptr);

private:
	char* m_pStart;
	char* m_pEnd;
	char* m_pCurrent;
};

#endif