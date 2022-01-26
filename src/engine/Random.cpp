#include "Random.h"
#include <cassert>
#include <cstring>


Random::Random(int numValues, std::default_random_engine& rGen) :
	rGen { rGen },
	weights (numValues, 0),
	defaultWeights (numValues, numValues), 	// set default weights
	last { -1 }
{
	assert(numValues > 0);
	Reset();
}

Random::Random(int numValues, const int* defaultWeights, std::default_random_engine& rGen) :
	rGen { rGen },
	weights (numValues, 0),
	defaultWeights (defaultWeights, defaultWeights + numValues), 	// set default weights
	last { -1 }
{
	assert(numValues > 0);
	Reset();
}

void Random::Reset()
{
	std::copy(defaultWeights.begin(), defaultWeights.end(), weights.begin());
	sumWeights = 0;
	for (int w : weights) 
	{
		sumWeights += w;
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
		for (int i = 0; i < (int)weights.size(); ++i)
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
