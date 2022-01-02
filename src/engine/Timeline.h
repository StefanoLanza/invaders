#pragma once

#include <functional>
#include <vector>

using EventId = int;

struct Event
{
	EventId     id;
	float       time;
	const void* data;
};

using EventCbk = std::function<void(const Event&)>;


class Timeline 
{
public:
	Timeline(EventCbk callback);

	void SetCallback(EventCbk callback);
	void SetEvents(const Event* events, size_t numEvents);
	void Restart();
	void JumpToEventId(EventId eventId);
	void Advance(float dt);
	float GetTime() const;
	bool IsOver() const;

private:
	const Event* events;
	size_t       numEvents;
	EventCbk     callback;
	int          currEventIndex;
	float        t;
};
