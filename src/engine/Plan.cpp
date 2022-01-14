#include "Plan.h"
#include <cstring>
#include <cmath>
#include <cassert>

namespace
{

void InitAction(PlanState& planState, const Plan& plan, int actionIdx)
{
	planState.actionIdx = actionIdx;
	planState.ticks = 0;
	planState.pathLength = 0;
	planState.pathNodeIdx = 0;
	planState.pathTicks = 0;
	planState.cosTheta = 0.f;
	planState.sinTheta = 0.f;
	const Action& currAction = plan.actions[planState.actionIdx];
	switch (currAction.id) 
	{
	case ActionId::rotate:
		assert(currAction.rotate.duration > 0);
	{	float radPerTick = (currAction.rotate.dtheta / 180.f * pi) / (currAction.rotate.duration);
		planState.cosTheta = std::cos(radPerTick);
		planState.sinTheta = std::sin(radPerTick);
		break;
	}
	case ActionId::path:
		planState.pathLength = (int)strlen(currAction.path.seq);
		break;
	default:
		break;
	};
}

Vector2D PlanNode(char action, float hspeed, float vspeed) 
{
	switch (action) 
	{
		case 'l':
			return { -hspeed, 0.f };
		case 'r':
			return { hspeed, 0.f };
			break;
		case 't':
			return { 0.f, -vspeed };
			break;
		case 'b':
			return { 0.f, vspeed };
			break;
		default:
			break;
	}
	return { 0.f, 0.f };
}

}


void InitPlan(PlanState& planState, const Plan& plan)
{
	planState.targetPos = { 0.f, 0.f };
	InitAction(planState, plan, 0);
}


void TickPlan(PlanState& planState, const Plan& plan, Vector2D& velocity, const Vector2D& pos)
{
	if (planState.actionIdx >= plan.actionCount)
	{
		return;
	}
	bool gotoNextAction = false;
	planState.ticks++;
	const Action& currAction = plan.actions[planState.actionIdx];
	switch (currAction.id)
	{
	case ActionId::move:
	{
		Vector2D diff = Sub({currAction.move.x, currAction.move.y}, pos);
		if (SquareLength(diff) < 1.f)
		{
			gotoNextAction = true;
			velocity = { 0.f , 0.f};
		}
		else
		{
			velocity = Normalize(diff, currAction.move.speed);
		}
		break;
	}
	case ActionId::translate:
		velocity = {currAction.translate.dx, currAction.translate.dy};
		if (planState.ticks >= currAction.translate.duration)
		{
			gotoNextAction = true;
		}
		break;
	case ActionId::rotate:
	{
		velocity = Rotate(velocity, planState.cosTheta, planState.sinTheta);
		if (planState.ticks >= currAction.rotate.duration)
		{
			gotoNextAction = true;
		}
	}
		break;
	case ActionId::path:
		velocity = PlanNode(currAction.path.seq[planState.pathNodeIdx], currAction.path.vx, currAction.path.vy);
		++planState.pathTicks;
		if (planState.pathTicks >= currAction.path.duration)
		{
			planState.pathTicks = 0;
			++planState.pathNodeIdx;
			if (planState.pathNodeIdx >= planState.pathLength) 
			{
				gotoNextAction = true;
			}
		}
		break;
	default:
		break;
	};

	if (gotoNextAction)
	{
		int actionIdx = planState.actionIdx + 1;
		if (actionIdx >= plan.actionCount)
		{
			if (plan.loop)
			{
				actionIdx = 0;
			}
			else
			{
				return;
			}
		}
		InitAction(planState, plan, actionIdx);
	}
}
