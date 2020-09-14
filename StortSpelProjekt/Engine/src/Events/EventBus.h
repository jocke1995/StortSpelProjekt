#pragma once
#include <list>
#include <map>
#include <typeindex>
#include "MemberFunctionWrapper.h"
#include "Events.h"

typedef std::list<HandlerFunctionBase*> HandlerList;
class EventBus 
{
public:
	template<typename EventType>
	void Publish(EventType* evnt);

	template<class T, class EventType>
	void Subscribe(T* classInstance, void (T::* memberFunction)(EventType*));

	//Singleton of the eventbus
	static EventBus& GetInstance();

	template<class T, class EventType>
	void unsubscribe(T* instance, void (T::* memberFunction)(EventType*));

	void unsubscribeAll();

	// Only called when program exits
	~EventBus();
private:
	std::map<std::type_index, HandlerList*> m_Subscribers;
};

// Publish to the EventBus using an event from Events.h
template<typename EventType>
inline void EventBus::Publish(EventType* evnt)
{
	//Get all subscribed handlers associated with the current event
	HandlerList* handlers = m_Subscribers[typeid(EventType)];

	//No handler exists, exit
	if (handlers == nullptr)
	{
		return;
	}

	//Loop through each handler and execute events
	for (auto& handler : *handlers) 
	{
		if (handler != nullptr) 
		{
			handler->Exec(evnt);
		}
	}
}

// Subscribe to the EventBus
template<class T, class EventType>
inline void EventBus::Subscribe(T* classInstance, void(T::* memberFunction)(EventType*))
{
	//Get all subscribed handlers associated with the current event
	HandlerList* handlers = m_Subscribers[typeid(EventType)];

	//If there are no handlers for the current EventType, create one
	if (handlers == nullptr)
	{
		handlers = new HandlerList();
		m_Subscribers[typeid(EventType)] = handlers;
	}

	//Push handler into list of handlers
	handlers->push_back(new MemberFunctionHandler<T, EventType>(classInstance, memberFunction));
}

// Get the single instance of the EventBus in order to subscribe/publish
inline EventBus& EventBus::GetInstance()
{
	static EventBus instance;
	return instance;
}

inline EventBus::~EventBus()
{
	for (auto const& i : m_Subscribers)
	{
		HandlerList* handlers = i.second;
		if (handlers != nullptr)
		{
			std::list<HandlerFunctionBase*>::iterator iter;
			for (iter = handlers->begin(); iter != handlers->end(); iter++)
			{
				delete* iter;
			}
			delete handlers;
		}
	}
	m_Subscribers.clear();
}
