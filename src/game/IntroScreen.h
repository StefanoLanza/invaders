#pragma once


struct Game;
class Console;


// FIXME Add constructor
void EnterIntroScreen(void* data, Game& game, int currentState);
int IntroScreen(Game& game, void* data, float dt);
void DisplayIntroScreen(Console& renderer, const void* data);

struct IntroScreenData;
extern IntroScreenData introScreenData;