#include "ActionMgr.h"
#include <algorithm>
#include <limits>
#include <cassert>

struct ActionMgr::Action {
	int*       counter;
	float      delay;
	float      duration;
	float      t;
	ActionFunc func;
};

ActionMgr::ActionMgr() = default;

ActionMgr::~ActionMgr() = default;

void ActionMgr::AddAction(int* counter, float delay, ActionFunc&& func) {
	AddTimedAction(counter, delay, std::numeric_limits<float>::max(), std::move(func));
}

void ActionMgr::AddTimedAction(int* counter, float delay, float duration, ActionFunc&& func) {
	assert(duration >= 0.f);
	if (counter) {
		++*counter;
	}
	actions.push_back({ counter, delay, duration, 0.f, std::move(func) });
}

void ActionMgr::RunActions(float dt) {
	auto pred = [dt](Action& Action) {
		bool res = false;
		Action.delay -= dt;
		if (Action.delay < 0.f) {
			Action.t += dt;
			float t01 = std::clamp(Action.t / Action.duration, 0.f, 1.f);
			res = Action.func(dt, t01);
			res = res || (Action.t >= Action.duration);
			if (res && Action.counter) {
				--*Action.counter; // Decrease counter, to inform client
			}
		}
		return res;
	};
	// Run Actions and remove those that have completed
	actions.erase(std::remove_if(actions.begin(), actions.end(), pred), actions.end());
}

void ActionMgr::Clear() {
	actions.clear();
}

bool ActionMgr::AnyRunning() const {
	return actions.empty() == false;
}
