#include "Plan.h"
#include <cstring>
#include <cmath>

namespace
{

void InitAction(PlanState& planState, int actionIdx)
{
	const Plan& plan = planState.plan;
	planState.actionIdx = actionIdx;
	planState.ticks = 0;
	planState.pathLength = 0;
	planState.cosTheta = 0.f;
	planState.sinTheta = 0.f;
	const Action& currAction = plan.actions[planState.actionIdx];
	switch (currAction.id) 
	{
	case ActionId::rotate:
		planState.cosTheta = std::cos(currAction.rotate.dtheta);
		planState.sinTheta = std::sin(currAction.rotate.dtheta);
		break;
	case ActionId::path:
		planState.pathLength = (int)strlen(currAction.path.seq);
		break;
	default:
		break;
	};
}

}


void InitPlan(PlanState& planState, const Plan& plan)
{
	planState.plan = plan;
	InitAction(planState, 0);
}


void TickPlan(PlanState& planState, Vector2D& velocity, const Vector2D& pos)
{
	const Plan& plan = planState.plan;
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
		if (planState.ticks > currAction.translate.ticks)
		{
			gotoNextAction = true;
		}
		break;
	case ActionId::rotate:
	{
		velocity = Rotate(velocity, planState.cosTheta, planState.sinTheta);
		if (planState.ticks > currAction.rotate.ticks)
		{
			gotoNextAction = true;
		}
	}
		break;
	case ActionId::path:
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
		InitAction(planState, actionIdx);
	}
}
