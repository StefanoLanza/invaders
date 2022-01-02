#include "Timeline.h"
#include <algorithm>
#include <limits>
#include <cassert>

Timeline::Timeline() :
	events(nullptr),
	numEvents(0),
	callback(nullptr),
	currEventIndex(0),
	t(0.f)
{
}

void Timeline::SetCallback(EventCbk callback_) 
{
	callback = std::move(callback_);
}

void Timeline::SetEvents(const Event* events_, size_t numEvents_) 
{
	events = events_;
	numEvents = numEvents_;
	Restart();
}

void Timeline::Restart()
{
	currEventIndex = 0;
	t = 0.f;
}

void Timeline::JumpToEventId(EventId eventId) 
{
	while (currEventIndex < numEvents && events[currEventIndex].id != eventId)
	{
		t = events[currEventIndex].time;
		if (callback) 
		{
			callback(events[currEventIndex]);
		}
		++currEventIndex;
	}
}


void Timeline::Advance(float dt) 
{
	t += dt;
	while (currEventIndex < numEvents && t > events[currEventIndex].time)
	{
		if (callback) 
		{
			callback(events[currEventIndex]);
		}
		++currEventIndex;
	}
}

float Timeline::GetTime() const 
{
	return t;
}

bool Timeline::IsOver() const 
{
	return currEventIndex >= numEvents;
}
