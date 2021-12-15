#pragma once

#include <random>


enum class KeyCode
{
	escape,
	enter,
	spaceBar,
	left,
	right,
	up,
	down,
	_1,
	_2,
	_3,
	_4,
	_5,
	A,
	D
};
constexpr size_t numKeyCodes = static_cast<size_t>(KeyCode::D) + 1;


void UpdateKeyStates();
bool KeyPressed(KeyCode keyCode);
bool KeyJustPressed(KeyCode keyCode);
bool KeyJustReleased(KeyCode keyCode);
bool AnyKeyJustPressed();


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


