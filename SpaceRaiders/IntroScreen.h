#pragma once


class Game;
class Renderer;


int IntroScreen(Game& game, void* data, float dt);
void DisplayIntroScreen(Renderer& renderer, const void* data);

struct IntroScreenData;
extern IntroScreenData introScreenData;