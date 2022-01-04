#include "Random.h"
#include <cassert>
#include <cstring>


Random::Random(int numValues, std::default_random_engine& rGen) :
	rGen { rGen },
	rndInt { 0, numValues - 1 }
{
	assert(numValues > 0 && numValues < maxValues);
	Reset();
}


void Random::Reset()
{
	for (int i = 0; i <= rndInt.max(); ++i) {
		available[i] = i;
	}
	numCandidates = rndInt.max() + 1;
}


int Random::Next()
{
	// TODO insert last in candidates
	const int r = rndInt(rGen) % numCandidates;
	const int n = available[r];
	if (numCandidates == 1) 
	{
		Reset();
		// Remove n from candidates
		available[n] = available[numCandidates - 1];
	}
	else 
	{
		// Remove n from candidates
		available[r] = available[numCandidates - 1];
	}
	--numCandidates;
	return n;
}
