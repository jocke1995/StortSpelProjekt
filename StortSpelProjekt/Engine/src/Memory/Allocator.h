#ifndef ALLOCATOR_H
#define ALLOCATOR_H

class Allocator
{
public:
	Allocator();
	virtual ~Allocator();
	virtual void* Allocate(size_t size) = 0;
	virtual void Free(void* ptr) = 0;
};

#endif