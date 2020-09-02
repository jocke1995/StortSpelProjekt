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

void* MemoryManager::AllocStackBlock()
{
    MemoryManager& mm = MemoryManager::getInstance();
    void* toReturn = mm.m_pStackEnd->pMem;
    if (mm.m_pStackEnd == mm.m_pHeapHead)
        return nullptr;
    mm.m_pStackEnd = mm.m_pStackEnd + 1;
    return toReturn;
}

void MemoryManager::FreeHeapBlock(void* ptr)
{
    MemoryManager& mm = MemoryManager::getInstance();
    size_t relPtr = (char*)ptr - (char*)MemoryManager::getInstance().m_pMem;
    size_t blockNr = relPtr / mm.GetBlockSize();
    Block* block = ((Block*)mm.m_pMem + blockNr);
    block->pNext = mm.m_pHeapHead;
    mm.m_pHeapHead = block;
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
