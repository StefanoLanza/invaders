#pragma once

#include <functional>
#include <vector>

// Returns true if complete
using EventFunc = std::function<bool(float dt, float t)>;

class EventMgr {
public:
	EventMgr();
	~EventMgr();

	void AddEvent(int* counter, float delay, EventFunc&& func);
	void AddTimedEvent(int* counter, float delay, float duration, EventFunc&& func);
	void RunEvents(float dt);
	void Clear();
	bool AnyRunning() const;

private:
	struct Event;
	std::vector<Event> events;
};
