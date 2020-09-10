#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

// This memorymanager allocates memory much like a double ended stack.
// The memory manager holds a large amount of memory which is used by the program.
// This memory is divided into three parts. Metadata, stack and heap.

// The metadata is any specific information which the memorymanager needs, such as blocks.

// The stack is growing from low memory adresses to higher addresses. It may only grow or decrease, no blocks of memory may be freed in the middle of it. This makes it safe 
// from fragmentation as long as noone tries to free memory in the middle of the stack block.

// The heap is growing from high memory adresses to lower adresses. It is allowed to free memory blocks anywhere where memory is allocated.
// The implementation will then be like that of a Pool Allocator to get a constant allocation and deallocation time. The highest memory adresses are the ones that should be returned from this part of the memory.

struct Block
{
	// ptrs are 8 bytes each. 16 bytes in total.
	void* pMem;
	Block* pNext;
};

class MemoryManager
{
public:
	~MemoryManager();
	static void* AllocHeapBlock();
	static void* AllocHeap(size_t size);
	static void* AllocStackBlock();
	static void* AllocStack(size_t size);

	static void FreeHeapBlock(void* ptr);
	static void FreeStackBlock(void* ptr);

	static constexpr size_t GetBlockSize();
	static constexpr size_t GetTotMemSize();
private:
	MemoryManager();

	Block* m_pStackEnd;
	Block* m_pHeapHead;
	void* m_pMem;

	static constexpr size_t getNrOfBlocks();
	static constexpr size_t getMetadataSizeInBlocks();
	static MemoryManager& getInstance();
};

#endif