#include "StackAllocator.h"

StackAllocator::StackAllocator(void* start, void* end)
{
    m_pStart = static_cast<char*>(start);
    m_pCurrent = m_pStart;
    m_pEnd = static_cast<char*>(end);
}

void* StackAllocator::Allocate(size_t size)
{
    if (m_pCurrent + size > m_pEnd)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "Stack Allocator failed to allocate memory, no memory left!\n");
        return nullptr; // No memory left, allocation fails.
    }

    m_pCurrent += size;
    return m_pCurrent;
}

void StackAllocator::Free(void* ptr)
{
    if (ptr == nullptr)
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "Stack Allocator failed to free memory!\n");
    }
    else if (ptr < m_pCurrent && ptr < m_pEnd)
    {
        m_pCurrent = static_cast<char*>(ptr);
    }
    else
    {
        Log::PrintSeverity(Log::Severity::CRITICAL, "Stack Allocator free() error!\n");
    }
}
