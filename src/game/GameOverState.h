#pragma once


struct Game;
class Console;


int GameOverMenu(Game& game, void* data, float dt);
void DisplayGameOver(Console& renderer, const void* gameState);
