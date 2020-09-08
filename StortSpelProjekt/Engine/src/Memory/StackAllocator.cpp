#include "StackAllocator.h"
#include "MemoryManager.h"

StackAllocator::StackAllocator()
{
    m_pStart = static_cast<char*>(MemoryManager::AllocStackBlock());
    m_pCurrent = m_pStart;
    m_pEnd = static_cast<char*>(m_pStart + MemoryManager::GetBlockSize());
}

void* StackAllocator::Allocate(size_t size)
{
    StackAllocator& sa = getInstace();
    if (sa.m_pCurrent + size > sa.m_pEnd)
    {
        size_t sizeToAskFor = Max(size, MemoryManager::GetBlockSize());
        void * newPtr = MemoryManager::AllocStack(sizeToAskFor);

        if (newPtr && static_cast<char*>(newPtr) == sa.m_pEnd)
        {
            sa.m_pEnd = static_cast<char*>(newPtr) + static_cast<int>(1 + static_cast<float>(sizeToAskFor / MemoryManager::GetBlockSize())) * MemoryManager::GetBlockSize();
        }
        else
        {
            if (newPtr)
            {
                MemoryManager::FreeStackBlock(newPtr);
            }

            Log::PrintSeverity(Log::Severity::CRITICAL, "Stack Allocator failed to allocate memory, no memory left!\n");
            return nullptr; // No memory left, allocation fails.
        }
    }

    void* toRet = sa.m_pCurrent;
    sa.m_pCurrent += size;
    return toRet;
}

void StackAllocator::Free(void* ptr)
{
    StackAllocator& sa = StackAllocator::getInstace();
    if (ptr == nullptr)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "Stack Allocator failed to free memory!\n");
    }
    else if (ptr < sa.m_pCurrent && ptr < sa.m_pEnd)
    {
        sa.m_pCurrent = static_cast<char*>(ptr);
    }
    else
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "Stack Allocator free() error!\n");
    }
}

StackAllocator& StackAllocator::getInstace()
{
    static StackAllocator instance;
    return instance;
}
