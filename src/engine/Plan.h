#pragma once

#include <engine/Vector2D.h>

enum class ActionId
{
	move,
	translate,
	rotate,
	path,
};

struct Action
{
	ActionId id;
	union
	{
		struct
		{
			float x, y;
			float speed; // per tick?
		} move;
		struct
		{
			float dx;
			float dy;
			int ticks;
		} translate;
		struct
		{
			float dtheta; // per tick
			int ticks;
		} rotate;
		struct
		{
			const char* seq;
		} path;
	};
};

struct Plan
{
	const Action* actions;
	int actionCount;
	bool loop;
};

struct PlanState
{
	Plan plan;
	int actionIdx;
	// For current action
	int ticks;
	int pathLength;
	float cosTheta;
	float sinTheta;
};

void InitPlan(PlanState& planState, const Plan& plan);
void TickPlan(PlanState& planState, Vector2D& velocity, const Vector2D& pos);
