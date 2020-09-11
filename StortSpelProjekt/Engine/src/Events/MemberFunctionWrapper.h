#pragma once
#include "Events.h"

// This is the interface for MemberFunctionWrapper that each specialization will use
class HandlerFunctionBase 
{
public:
    // Call the member function
    void Exec(Event* evnt) 
    {
        Call(evnt);
    }
private:
    // Implemented by MemberFunctionHandler
    virtual void Call(Event* evnt) = 0;
};

template<class T, class EventType>
class MemberFunctionHandler : public HandlerFunctionBase
{
public:
    typedef void (T::* MemberFunction)(EventType*);

    MemberFunctionHandler(T* m_pInstance, MemberFunction m_MemberFunction) : m_pInstance{ m_pInstance }, m_MemberFunction{ m_MemberFunction } {};

  
private:
    void Call(Event* evnt)
    {
        // Cast event to the correct type and call member function
        (m_pInstance->*m_MemberFunction)(static_cast<EventType*>(evnt));
    }

    // Pointer to class instance
    T* m_pInstance;

    // Pointer to member function
    MemberFunction m_MemberFunction;
};