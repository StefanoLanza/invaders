#pragma once


struct Game;
class Console;


int VictoryScreen(Game& game, void* data, float dt);
void DisplayVictoryScreen(Console& renderer, const void* data);
