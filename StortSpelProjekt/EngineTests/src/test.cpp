#include <gtest/gtest.h>
#include "Headers/stdafx.h"
#include <crtdbg.h>
#include "Headers/Core.h"
#include "Memory/FreeList.h"
#include "Memory/MemoryManager.h"
#include "Misc/EngineRand.h"
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
/*
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
*/

TEST(FREELIST, ALLOCELEMENT)
{
	// INIT
	size_t toAlloc = sizeof(Entry) + sizeof(int);

	// TEST
	int* element = (int*)FreeList::Allocate(sizeof(int));

	EXPECT_NE(element, nullptr);

	// RESET
	FreeList::Free(element);
}

TEST(FREELIST, ALLOCMANY)
{
	// INIT
	size_t nrOfElements = 5;

	// TEST
	int* elements = (int*)FreeList::Allocate(sizeof(int) * nrOfElements);
	EXPECT_NE(nullptr, elements);

	for (int i = 0; i < nrOfElements; i++)
	{
		elements[i] = i;
	}


	// RESET
	FreeList::Free(elements);
}

TEST(FREELIST, ALLOCANDFREE)
{
	// INIT
	size_t nrOfElements = 100;
	size_t toAlloc = (sizeof(Entry) + sizeof(int)) * nrOfElements + sizeof(Entry) + (sizeof(int*)) * nrOfElements;

	// TEST
	int** elementsPtr = (int**)FreeList::Allocate(sizeof(int*) * nrOfElements);
	EXPECT_NE(nullptr, elementsPtr);

	if (elementsPtr)
	{
		for (int i = 0; i < nrOfElements; i++)
		{
			elementsPtr[i] = (int*)FreeList::Allocate(sizeof(int));
			*elementsPtr[i] = i;
		}

		// Because of the properties of the freelist, it should go faster to free from the back
		// as parents try to merge with their children.
		for (int i = nrOfElements - 1; i >= 0; i--)
		{
			FreeList::Free(elementsPtr[i]);
		}
		FreeList::Free(elementsPtr);

		// Once everythin is deallocated we should have room to allocate all of the memory minus the size of an entry

		void* largerAlloc = FreeList::Allocate(toAlloc - sizeof(Entry));

		EXPECT_NE(largerAlloc, nullptr);

		// RESET
		FreeList::Free(largerAlloc);
	}
}

class memTester
{
public:
	inline memTester(int id) { memset(m_Bytes, 0, 1024); m_pDynamicInteger = (int*)FreeList::Allocate(sizeof(int)); *m_pDynamicInteger = id; };
	inline ~memTester() { Log::Print("memTester with ID %d deleted.\n", *m_pDynamicInteger); FreeList::Free(m_pDynamicInteger); };
	inline int GetId() { return *m_pDynamicInteger; };
private:
	char m_Bytes[1024];
	int* m_pDynamicInteger;
};

TEST(FREELIST, CUSTOMCLASS)
{
	size_t nrOfElements = 100;

	memTester** ptrs = (memTester**)FreeList::Allocate(nrOfElements * sizeof(memTester*));
	EXPECT_NE(nullptr, ptrs);
	if (ptrs)
	{
		for (int i = 0; i < nrOfElements; i++)
		{
			ptrs[i] = FreeList::Allocate<memTester>(i);
			int tst = ptrs[i]->GetId();
			Log::Print("Id of memTester: %d\n", tst);
			EXPECT_NE(nullptr, ptrs[i]);
		}

		// Free in a non contigous order.
		for (int i = 0; i < nrOfElements / 4; i++)
		{
			if (ptrs[i])
			{
				FreeList::Free(ptrs[i]);
			}
		}

		for (int i = nrOfElements / 2; i >= nrOfElements / 4; i--)
		{
			if (ptrs[i])
			{
				FreeList::Free(ptrs[i]);
			}
		}

		for (int i = nrOfElements - 1; i > nrOfElements / 2; i--)
		{
			if (ptrs[i])
			{
				FreeList::Free(ptrs[i]);
			}
		}
		FreeList::Free(ptrs);
	}
}

// ------------------ MemoryManager ------------------
TEST(MEMORYMANAGER, STACKBASIC)
{
	void* block = MemoryManager::AllocStackBlock();
	EXPECT_NE(block, nullptr);
	
	if(block)
		MemoryManager::FreeStackBlock(block);
}

TEST(MEMORYMANAGER, STACKFEW)
{
	void* blockFirst = MemoryManager::AllocStackBlock();
	void* blockSecond = MemoryManager::AllocStackBlock();
	void* blockThird = MemoryManager::AllocStackBlock();
	void* blockFourth = MemoryManager::AllocStackBlock();

	EXPECT_NE(blockFirst, nullptr);
	EXPECT_NE(blockSecond, nullptr);
	EXPECT_NE(blockThird, nullptr);
	EXPECT_NE(blockFourth, nullptr);
	
	if(blockFourth)
		MemoryManager::FreeStackBlock(blockFourth);
	if(blockThird)
		MemoryManager::FreeStackBlock(blockThird);
	if(blockSecond)
		MemoryManager::FreeStackBlock(blockSecond);
	if(blockFirst)
		MemoryManager::FreeStackBlock(blockFirst);
}

TEST(MEMORYMANAGER, STACKMANY)
{
	void** blockWithBlocks = (void**)MemoryManager::AllocStackBlock();

	EXPECT_NE(nullptr, blockWithBlocks);
	if (blockWithBlocks)
	{
		size_t toAlloc = MemoryManager::GetBlockSize() / 8;
		for (int i = 0; i < toAlloc; i++)
		{
			blockWithBlocks[i] = nullptr;
			blockWithBlocks[i] = MemoryManager::AllocStackBlock();
			EXPECT_NE(blockWithBlocks[i], nullptr);
		}

		for (int i = toAlloc - 1; i >= 0; i--)
		{
			if(blockWithBlocks[i])
				MemoryManager::FreeStackBlock(blockWithBlocks[i]);
		}
		MemoryManager::FreeStackBlock(blockWithBlocks);
	}
}

TEST(MEMORYMANAGER, HEAPBASIC)
{
	void* block = MemoryManager::AllocHeapBlock();
	EXPECT_NE(nullptr, block);
	if(block)
		MemoryManager::FreeHeapBlock(block);
}

TEST(MEMORYMANAGER, HEAPFEW)
{
	void* blockFirst = MemoryManager::AllocHeapBlock();
	void* blockSecond = MemoryManager::AllocHeapBlock();
	void* blockThird = MemoryManager::AllocHeapBlock();
	void* blockFourth = MemoryManager::AllocHeapBlock();

	EXPECT_NE(blockFirst, nullptr);
	EXPECT_NE(blockSecond, nullptr);
	EXPECT_NE(blockThird, nullptr);
	EXPECT_NE(blockFourth, nullptr);


	// Dealloc in random order.
	if(blockThird)
		MemoryManager::FreeHeapBlock(blockThird);
	if(blockFourth)
		MemoryManager::FreeHeapBlock(blockFourth);
	if(blockFirst)
		MemoryManager::FreeHeapBlock(blockFirst);
	if(blockSecond)
		MemoryManager::FreeHeapBlock(blockSecond);
}

TEST(MEMORYMANAGER, HEAPMANY)
{
	void** blockWithBlocks = (void**)MemoryManager::AllocHeapBlock();

	EXPECT_NE(blockWithBlocks, nullptr);

	if (blockWithBlocks)
	{
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
}

TEST(MEMORYMANAGER, HEAPLARGE)
{
	void* mem = MemoryManager::AllocHeap(MemoryManager::GetBlockSize() * 2);
	EXPECT_NE(mem, nullptr);
	
	if(mem)
		MemoryManager::FreeHeapBlock(mem);
}

TEST(MEMORYMANAGER, HEAPFEWANDLARGE)
{
	void* smallMem = MemoryManager::AllocHeapBlock();
	void* smallMemSec = MemoryManager::AllocHeapBlock();

	EXPECT_NE(smallMem, nullptr);
	EXPECT_NE(smallMemSec, nullptr);
	
	if(smallMem)
		MemoryManager::FreeHeapBlock(smallMem);
	
	void* mem = MemoryManager::AllocHeap(MemoryManager::GetBlockSize() * 2);
	EXPECT_NE(mem, nullptr);

	smallMem = MemoryManager::AllocHeapBlock();
	EXPECT_NE(smallMem, nullptr);

	if(smallMemSec)
		MemoryManager::FreeHeapBlock(smallMemSec);

	smallMemSec = MemoryManager::AllocHeap(MemoryManager::GetBlockSize() * 100);
	EXPECT_NE(smallMemSec, nullptr);
	if(smallMemSec)
		MemoryManager::FreeHeapBlock(smallMemSec);
	if (smallMem)
		MemoryManager::FreeHeapBlock(smallMem);
	if(mem)
		MemoryManager::FreeHeapBlock(mem);
}

TEST(RANDOMGEN, BASIC)
{
	EngineRand lehm(5);
	Log::Print("\nRandom tests\n\n");

	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
	Log::Print("%d\n", lehm.Rand());
}

TEST(RANDOMGEN, RANGE)
{
	EngineRand lehm(2);

	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
	Log::Print("%d\n", lehm.Rand(1, 10));
}

TEST(RANDOMGEN, FLOATS)
{
	EngineRand lehm(5);

	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
	Log::Print("%f\n", lehm.Randf());
}

TEST(RANDOMGEN, FLOATSRANGE)
{
	EngineRand lehm(2);

	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
	Log::Print("%f\n", lehm.Randf(1, 10));
}