#include "stdafx.h"
#include "MemoryManager.h"

MemoryManager::~MemoryManager()
{
    free(m_pMem);
}

void* MemoryManager::AllocHeapBlock()
{
    MemoryManager& mm = MemoryManager::getInstance();
    void* toReturn = mm.m_pHeapHead->pMem;
    if (mm.m_pStackEnd == mm.m_pHeapHead)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "MemoryManager out of memory!\n");
        return nullptr;
    }
    Block* targetBlock = mm.m_pHeapHead;
    mm.m_pHeapHead = targetBlock->pNext;
    targetBlock->pNext = targetBlock;

#ifdef _DEBUG
    if (!toReturn)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "MemoryManager could not hand out memory!\n");
    }
#endif // _DEBUG

    return toReturn;
}

void* MemoryManager::AllocHeap(size_t size)
{
    MemoryManager& mm = MemoryManager::getInstance();
    void* toReturn = nullptr;
    size_t nrOfBlocksToAlloc = 1 + ((size - 1) / MemoryManager::GetBlockSize());
    bool nFound = true;
    Block* candidate = mm.m_pHeapHead;
    Block* candChild = candidate;

    while (nFound)
    {
        candidate = candChild;
        nFound = candChild->pNext != candChild - 1;

        if (nFound)
        {
            candChild = candChild->pNext;
        }

        for (int i = 1; i < nrOfBlocksToAlloc && !nFound && candChild != mm.m_pStackEnd; i++)
        {
            candChild = candChild->pNext;
            nFound = candChild->pNext != candChild - 1;
        }
    }

    if (candChild == mm.m_pStackEnd)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "MemoryManager out of memory!\n");
        return nullptr;
    }

    if (candidate)
    {
        toReturn = candChild->pMem;
        if (candidate == mm.m_pHeapHead)
        {
            mm.m_pHeapHead = candChild->pNext;
        }
        else
        {
            mm.m_pHeapHead->pNext = candChild->pNext;
        }
        candChild->pNext = candidate;
    }
#ifdef _DEBUG
    if (!toReturn)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "MemoryManager could not hand out memory!\n");
    }
#endif // _DEBUG

    return toReturn;
}

void* MemoryManager::AllocStackBlock()
{
    MemoryManager& mm = MemoryManager::getInstance();
    void* toReturn = mm.m_pStackEnd->pMem;
    if (mm.m_pStackEnd == mm.m_pHeapHead)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "MemoryManager out of memory!\n");
        return nullptr;
    }
    mm.m_pStackEnd = mm.m_pStackEnd + 1;

#ifdef _DEBUG
    if (!toReturn)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "MemoryManager could not hand out memory!\n");
    }
#endif // _DEBUG

    return toReturn;
}

void* MemoryManager::AllocStack(size_t size)
{
    MemoryManager& mm = MemoryManager::getInstance();

    // Calculate how many blocks are needed.
    size_t nrOfBlocksToAlloc = 1 + (size / MemoryManager::GetBlockSize());

    void* toReturn = mm.m_pStackEnd->pMem;
    if (mm.m_pStackEnd + nrOfBlocksToAlloc == mm.m_pHeapHead)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "MemoryManager out of memory!\n");
        return nullptr;
    }
    mm.m_pStackEnd = mm.m_pStackEnd + nrOfBlocksToAlloc;

#ifdef _DEBUG
    if (!toReturn)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "MemoryManager could not hand out memory!\n");
    }
#endif // _DEBUG

    return toReturn;
}

// TODO: "Free" all blocks that are linked to the given memory until nullptr is reached.
void MemoryManager::FreeHeapBlock(void* ptr)
{
    MemoryManager& mm = MemoryManager::getInstance();
    size_t relPtr = (char*)ptr - (char*)MemoryManager::getInstance().m_pMem;
    size_t blockNr = relPtr / mm.GetBlockSize();
    Block* block = ((Block*)mm.m_pMem + blockNr);
    
    if (block < mm.m_pHeapHead)
    {
        // Insert it.
        Block* toCompare = mm.m_pHeapHead;

        while (block < toCompare->pNext)
        {
            toCompare = toCompare->pNext;
        }
        Block* blockTarget = toCompare->pNext;
        toCompare->pNext = block->pNext;
        block->pNext = blockTarget;
    }
    else
    {
        Block* blockTarget = mm.m_pHeapHead;
        mm.m_pHeapHead = block->pNext;
        block->pNext = blockTarget;
    }
}

void MemoryManager::FreeStackBlock(void* ptr)
{
    MemoryManager& mm = MemoryManager::getInstance();
    size_t relPtr = (char*)ptr - (char*)MemoryManager::getInstance().m_pMem;
    size_t blockNr = relPtr / mm.GetBlockSize();
    Block* block = ((Block*)mm.m_pMem + blockNr);
    
#ifdef _DEBUG
    if (block < mm.m_pStackEnd - 1)
    {
        Log::PrintSeverity(Log::Severity::WARNING, "Stack freed more than one block!\n");
    }
    else if (block >= mm.m_pStackEnd)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "Stack tried to free block which it does not hold!\n");
        return;
    }
#endif // _DEBUG

    mm.m_pStackEnd = block;
}

MemoryManager::MemoryManager() : m_pStackEnd(nullptr), m_pHeapHead(nullptr)
{
    m_pMem = malloc(MemoryManager::GetTotMemSize());

    Block* lastBlock = new (m_pMem) Block;
    lastBlock->pNext = nullptr;
    lastBlock->pMem = m_pMem;

    Block* blockToInit = nullptr;
    for (unsigned int i = 1; i < getNrOfBlocks(); i++)
    {
        // size of struct objekt "Block" is 16 bytes. (see MemoryManager.h)
        blockToInit = new ((char*)m_pMem + i * 16) Block;
        blockToInit->pNext = lastBlock;
        blockToInit->pMem = (char*)m_pMem + i * MemoryManager::GetBlockSize();
        lastBlock = blockToInit;
    }

    m_pStackEnd = (Block*)m_pMem + MemoryManager::getMetadataSizeInBlocks() + 1;
    m_pHeapHead = blockToInit;
    memset(m_pHeapHead->pMem, 0, GetBlockSize());
}

constexpr size_t MemoryManager::GetBlockSize()
{
    return 512;
}

constexpr size_t MemoryManager::GetTotMemSize()
{
    return 2000000;
}

constexpr size_t MemoryManager::getNrOfBlocks()
{
    return MemoryManager::GetTotMemSize() / MemoryManager::GetBlockSize();
}

constexpr size_t MemoryManager::getMetadataSizeInBlocks()
{
    // size of struct objekt "Block" is 16 bytes. (see MemoryManager.h)
    return MemoryManager::getNrOfBlocks() * 16 / MemoryManager::GetBlockSize();
}

MemoryManager& MemoryManager::getInstance()
{
    static MemoryManager instance;
    return instance;
}
