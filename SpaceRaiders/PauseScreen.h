#pragma once


class Game;
class Renderer;


int PauseScreen(Game& game, void* data, float dt);
void DisplayPauseScreen(Renderer& renderer, const void* data);
