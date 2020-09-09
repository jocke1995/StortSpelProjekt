#pragma once

class Event
{
public:
	virtual ~Event() = default;
};

struct MessageLog: public Event
{
	MessageLog(std::string msg) : msg{ msg } {};
	std::string msg;
};