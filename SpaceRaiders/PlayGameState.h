#pragma once


class Game;
struct RunGameStateData;

int PlayGame(Game& game, void* data, float dt);
void EnterPlayGame(void* data, int currentState);
extern RunGameStateData playGameStateData;
