#pragma once

#include <random>
#include "Keyboard.h"

class Input
{
public:

	virtual ~Input() = default;
	virtual void Update(float dt) = 0;
	virtual bool Left() const = 0;
	virtual bool Right() const = 0;
};

class RndInput final : public Input
{
public:	

	RndInput(std::default_random_engine& rGen);

	void Update(float dt) override;
	bool Left() const override;
	bool Right() const override;

private:

	std::default_random_engine& rGen;
	std::discrete_distribution<int> rndInt;
	float accumTime;
	int state;
};


class KeyboardInput final : public Input
{
public:	

	KeyboardInput(KeyCode left, KeyCode right);

	void Update(float dt) override;
	bool Left() const override;
	bool Right() const override;

private:

	KeyCode left;
	KeyCode right;
};


