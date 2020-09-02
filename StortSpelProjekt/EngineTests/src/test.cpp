#include <gtest/gtest.h>
#include <crtdbg.h>
#include "Memory/FreeList.h"
#include "Memory/MemoryManager.h"
int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(INITTEST, ZEROISZERO)
{
	EXPECT_EQ(0, 0);
}

// ------------------ FREELIST ------------------
TEST(FREELIST, ALLOCELEMENT)
{
	// INIT
	size_t toAlloc = sizeof(Entry) + sizeof(int);
	void* mem = malloc(toAlloc);
	FreeList allocator(mem, toAlloc);

	// TEST
	int* element = (int*)allocator.Allocate(sizeof(int));

	EXPECT_NE(element, nullptr);

	// RESET
	free(mem);
}

TEST(FREELIST, ALLOCMANY)
{
	// INIT
	size_t nrOfElements = 5;
	size_t toAlloc = (sizeof(Entry) + sizeof(int)) * nrOfElements;
	void* mem = malloc(toAlloc);
	FreeList allocator(mem, toAlloc);

	// TEST
	int* elements = (int*)allocator.Allocate(sizeof(int) * nrOfElements);

	for (int i = 0; i < nrOfElements; i++)
	{
		elements[i] = i;
	}

	allocator.Free(elements);

	// RESET
	free(mem);
}

TEST(FREELIST, ALLOCPTRS)
{
	// INIT
	size_t nrOfElements = 5;
	size_t toAlloc = (sizeof(Entry) + sizeof(int)) * nrOfElements + sizeof(Entry) + (sizeof(int*)) * nrOfElements;
	void* mem = malloc(toAlloc);
	FreeList allocator(mem, toAlloc);

	// TEST
	int** elementsPtr = (int**)allocator.Allocate(sizeof(int*) * nrOfElements);

	for (int i = 0; i < nrOfElements; i++)
	{
		elementsPtr[i] = (int*)allocator.Allocate(sizeof(int));
		*elementsPtr[i] = i;
	}

	// RESET
	free(mem);
}

TEST(FREELIST, ALLOCANDFREE)
{
	// INIT
	size_t nrOfElements = 5;
	size_t toAlloc = (sizeof(Entry) + sizeof(int)) * nrOfElements + sizeof(Entry) + (sizeof(int*)) * nrOfElements;
	void* mem = malloc(toAlloc);
	FreeList allocator(mem, toAlloc);

	// TEST
	int** elementsPtr = (int**)allocator.Allocate(sizeof(int*) * nrOfElements);

	for (int i = 0; i < nrOfElements; i++)
	{
		elementsPtr[i] = (int*)allocator.Allocate(sizeof(int));
		*elementsPtr[i] = i;
	}

	// Because of the properties of the freelist, it should go faster to free from the back
	// as parents try to merge with their children.
	for (int i = nrOfElements - 1; i >= 0; i--)
	{
		allocator.Free(elementsPtr[i]);
	}
	allocator.Free(elementsPtr);

	// Once everythin is deallocated we should have room to allocate all of the memory minus the size of an entry

	void* largerAlloc = allocator.Allocate(toAlloc - sizeof(Entry));

	EXPECT_NE(largerAlloc, nullptr);

	// RESET
	free(mem);
}

TEST(MALLOC, ALLOCANDFREE)
{
	// INIT
	size_t nrOfElements = 5;
	size_t toAlloc = (sizeof(Entry) + sizeof(int)) * nrOfElements + sizeof(Entry) + (sizeof(int*)) * nrOfElements;

	// TEST
	int** elementsPtr = (int**)malloc(sizeof(int*) * nrOfElements);

	for (int i = 0; i < nrOfElements; i++)
	{
		elementsPtr[i] = (int*)malloc(sizeof(int));
		*elementsPtr[i] = i;
	}

	// Because of the properties of the freelist, it should go faster to free from the back
	// as parents try to merge with their children.
	for (int i = nrOfElements - 1; i >= 0; i--)
	{
		free(elementsPtr[i]);
	}
	free(elementsPtr);

	// Once everythin is deallocated we should have room to allocate all of the memory minus the size of an entry

	void* largerAlloc = malloc(toAlloc - sizeof(Entry));

	EXPECT_NE(largerAlloc, nullptr);

	// RESET

	free(largerAlloc);
}

// ------------------ MemoryManager ------------------
TEST(MEMORYMANAGER, STACKBASIC)
{
	void* block = MemoryManager::AllocStackBlock();
	MemoryManager::FreeStackBlock(block);
}

TEST(MEMORYMANAGER, STACKFEW)
{
	void* blockFirst = MemoryManager::AllocStackBlock();
	void* blockSecond = MemoryManager::AllocStackBlock();
	void* blockThird = MemoryManager::AllocStackBlock();
	void* blockFourth = MemoryManager::AllocStackBlock();

	MemoryManager::FreeStackBlock(blockFourth);
	MemoryManager::FreeStackBlock(blockThird);
	MemoryManager::FreeStackBlock(blockSecond);
	MemoryManager::FreeStackBlock(blockFirst);
}

TEST(MEMORYMANAGER, STACKMANY)
{
	void** blockWithBlocks = (void**)MemoryManager::AllocStackBlock();
	size_t toAlloc = MemoryManager::GetBlockSize() / 8;
	for (int i = 0; i < toAlloc; i++)
	{
		blockWithBlocks[i] = nullptr;
		blockWithBlocks[i] = MemoryManager::AllocStackBlock();
		EXPECT_NE(blockWithBlocks[i], nullptr);
	}

	for (int i = toAlloc - 1; i >= 0; i--)
	{
		MemoryManager::FreeStackBlock(blockWithBlocks[i]);
	}
	MemoryManager::FreeStackBlock(blockWithBlocks);
}

TEST(MEMORYMANAGER, HEAPBASIC)
{
	void* block = MemoryManager::AllocHeapBlock();
	MemoryManager::FreeHeapBlock(block);
}

TEST(MEMORYMANAGER, HEAPFEW)
{
	void* blockFirst = MemoryManager::AllocHeapBlock();
	void* blockSecond = MemoryManager::AllocHeapBlock();
	void* blockThird = MemoryManager::AllocHeapBlock();
	void* blockFourth = MemoryManager::AllocHeapBlock();

	// Dealloc in random order.
	MemoryManager::FreeHeapBlock(blockThird);
	MemoryManager::FreeHeapBlock(blockFourth);
	MemoryManager::FreeHeapBlock(blockFirst);
	MemoryManager::FreeHeapBlock(blockSecond);
}

TEST(MEMORYMANAGER, HEAPMANY)
{
	void** blockWithBlocks = (void**)MemoryManager::AllocHeapBlock();
	size_t toAlloc = MemoryManager::GetBlockSize() / 8;
	for (int i = 0; i < toAlloc; i++)
	{
		blockWithBlocks[i] = nullptr;
		blockWithBlocks[i] = MemoryManager::AllocHeapBlock();
		EXPECT_NE(blockWithBlocks[i], nullptr);
	}

	for (int i = 0; i < toAlloc; i++)
	{
		MemoryManager::FreeHeapBlock(blockWithBlocks[i]);
	}

	MemoryManager::FreeHeapBlock(blockWithBlocks);
	
}