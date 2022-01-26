#pragma once

#include <random>
#include <vector>

class Random
{
public:
	Random(int numValues, std::default_random_engine &rGen);
	Random(int numValues, const int* weights, std::default_random_engine &rGen);

	void Reset();
	int Next();

private:

	std::default_random_engine &rGen;
	std::vector<int> weights;
	std::vector<int> defaultWeights;
	int sumWeights;
	int last;
};