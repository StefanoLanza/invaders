#pragma once


struct Game;
class Console;


int PauseScreen(Game& game, void* data, float dt);
void DisplayPauseScreen(Console& renderer, const void* data);
