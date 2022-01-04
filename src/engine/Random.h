#pragma once

#include <random>

class Random
{
public:
	Random(int numValues, std::default_random_engine &rGen);

	void Reset();
	int Next();

private:
	static constexpr int maxValues = 32;

	std::default_random_engine &rGen;
	int numValues;
	int weights[maxValues];
	int sumWeights;
	int last;
};