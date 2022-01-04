#include "Random.h"
#include <cassert>
#include <cstring>


Random::Random(int numValues, std::default_random_engine& rGen) :
	rGen { rGen },
	rndInt { 0, numValues - 1 },
	history {},
	last {-1}
{
	assert(numValues > 0 && numValues < maxValues);
}


void Random::Reset()
{
	std::memset(history, 0, sizeof history);
	last = -1;
}


int Random::Next()
{
	int v = 0;
	// FIXME Better way to bias types?
	for (int i = 0; i < rndInt.max(); ++i)
	{
		v = rndInt(rGen);
		if (v != last)
		{
			// Never generated
			break;
		}
	}
	last = v;
	history[v]++;
	return v;
}
