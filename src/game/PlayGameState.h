#pragma once


struct Game;
class Console;
struct PlayGameStateData;

void EnterPlayGame(void* data, Game& game, int currentState);
int PlayGame(Game& game, void* data, float dt);
void DisplayPlayGame(Console& renderer, const void* data);
