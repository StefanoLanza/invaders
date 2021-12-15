#include "RenderItem.h"


void UpdateAnimation(AnimState& state, const Animation& anim, float dt)
{
	state.t += dt;
	if (state.t > anim.duration && anim.numFrames)
	{
		int nextFrame = (state.frame + 1) % anim.numFrames;
		if (anim.images[nextFrame] != ImageId::null)
		{
			state.frame = nextFrame;
			state.t = 0.f;
		}
	}
}
