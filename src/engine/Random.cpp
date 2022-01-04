#include "Random.h"
#include <cassert>
#include <cstring>


Random::Random(int numValues, std::default_random_engine& rGen) :
	rGen { rGen },
	numValues { numValues },
	last { -1 }
{
	assert(numValues > 0 && numValues < maxValues);
	Reset();
}


void Random::Reset()
{
	sumWeights = 0;
	for (int i = 0; i < numValues; ++i) 
	{
		weights[i] = numValues;
		sumWeights += weights[i];
	}
	assert(sumWeights > 0);
	last = -1;
}


int Random::Next()
{
	if (sumWeights == 0)
	{
		Reset();
	}

	std::uniform_int_distribution<int> rndInt(0, sumWeights - 1);
	int n = -1;
	do
	{
		int r = rndInt(rGen);
		for (int i = 0; i < numValues; ++i)
		{
			if (r < weights[i])
			{
				n = i;
				break;
			}
			r -= weights[i];
		}
		assert(n != -1);
	} while(n == last);  // do not pick previous element
	weights[n]--;
	sumWeights--;
	last = n;

	return n;
}
