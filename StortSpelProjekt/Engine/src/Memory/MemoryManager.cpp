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
        return nullptr;
    mm.m_pHeapHead = mm.m_pHeapHead->pNext;
    return toReturn;
}

void* MemoryManager::AllocStackHeap()
{
    MemoryManager& mm = MemoryManager::getInstance();
    void* toReturn = mm.m_pStackEnd->pMem;
    if (mm.m_pStackEnd == mm.m_pHeapHead)
        return nullptr;
    // will this actually give a next block ptr?
    mm.m_pStackEnd = mm.m_pStackEnd + 1;
    return toReturn;
}

void MemoryManager::FreeHeapBlock(void* ptr)
{
    MemoryManager& mm = MemoryManager::getInstance();
    size_t relPtr = (char*)ptr - (char*)MemoryManager::getInstance().m_pMem;
    size_t blockNr = relPtr / mm.getBlockSize();
    Block* block = ((Block*)mm.m_pMem + blockNr);
    block->pNext = mm.m_pHeapHead;
    mm.m_pHeapHead = block;
}

void MemoryManager::FreeStackBlock(void* ptr)
{
    MemoryManager& mm = MemoryManager::getInstance();
    size_t relPtr = (char*)ptr - (char*)MemoryManager::getInstance().m_pMem;
    size_t blockNr = relPtr / mm.getBlockSize();
    Block* block = ((Block*)mm.m_pMem + blockNr);
    
#ifdef _DEBUG
    if (block < mm.m_pStackEnd - 1)
    {
        Log::PrintSeverity(Log::Severity::WARNING, "WARNING: Stack freed more than one block!\n");
    }
    else if (block >= mm.m_pStackEnd)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "ERROR: Stack freed block which it does not hold!\n");
    }
#endif // _DEBUG

    mm.m_pStackEnd = block;
}

MemoryManager::MemoryManager() : m_pStackEnd(nullptr), m_pHeapHead(nullptr)
{
    m_pMem = malloc(MemoryManager::getTotMemSize());

    Block* lastBlock = new (m_pMem) Block;
    lastBlock->pNext = nullptr;
    lastBlock->pMem = m_pMem;

    Block* blockToInit = nullptr;
    for (unsigned int i = 1; i < getNrOfBlocks(); i++)
    {
        // size of struct objekt "Block" is 16 bytes. (see MemoryManager.h)
        blockToInit = new ((char*)m_pMem + i * 16) Block;
        blockToInit->pNext = lastBlock;
        blockToInit->pMem = (char*)m_pMem + i * MemoryManager::getBlockSize();
    }

    m_pStackEnd = (Block*)m_pMem + MemoryManager::getMetadataSizeInBlocks();
    m_pHeapHead = blockToInit;
}

constexpr size_t MemoryManager::getBlockSize()
{
    return 512;
}

constexpr size_t MemoryManager::getTotMemSize()
{
    return 2000000;
}

constexpr size_t MemoryManager::getNrOfBlocks()
{
    return MemoryManager::getTotMemSize() / MemoryManager::getBlockSize();
}

constexpr size_t MemoryManager::getMetadataSizeInBlocks()
{
    // size of struct objekt "Block" is 16 bytes. (see MemoryManager.h)
    return MemoryManager::getNrOfBlocks() * 16 / MemoryManager::getBlockSize();
}

MemoryManager& MemoryManager::getInstance()
{
    static MemoryManager instance;
    return instance;
}
