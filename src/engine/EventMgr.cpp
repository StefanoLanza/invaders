#include "EventMgr.h"
#include <algorithm>
#include <limits>
#include <cassert>

struct EventMgr::Event {
	int*       counter;
	float      delay;
	float      duration;
	float      t;
	EventFunc func;
};

EventMgr::EventMgr() = default;

EventMgr::~EventMgr() = default;

void EventMgr::AddEvent(int* counter, float delay, EventFunc&& func) {
	AddTimedEvent(counter, delay, std::numeric_limits<float>::max(), std::move(func));
}

void EventMgr::AddTimedEvent(int* counter, float delay, float duration, EventFunc&& func) {
	assert(duration >= 0.f);
	if (counter) {
		++*counter;
	}
	events.push_back({ counter, delay, duration, 0.f, std::move(func) });
}

void EventMgr::RunEvents(float dt) {
	auto pred = [dt](Event& Event) {
		bool res = false;
		Event.delay -= dt;
		if (Event.delay < 0.f) {
			Event.t += dt;
			float t01 = std::clamp(Event.t / Event.duration, 0.f, 1.f);
			res = Event.func(dt, t01);
			res = res || (Event.t >= Event.duration);
			if (res && Event.counter) {
				--*Event.counter; // Decrease counter, to inform client
			}
		}
		return res;
	};
	// Run Events and remove those that have completed
	events.erase(std::remove_if(events.begin(), events.end(), pred), events.end());
}

void EventMgr::Clear() {
	events.clear();
}

bool EventMgr::AnyRunning() const {
	return events.empty() == false;
}
