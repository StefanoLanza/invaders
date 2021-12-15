#pragma once


struct Game;
class Renderer;


int VictoryScreen(Game& game, void* data, float dt);
void DisplayVictoryScreen(Renderer& renderer, const void* data);
