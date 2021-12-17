#pragma once


struct Game;
class Console;


int StartMenu(Game& game, void* data, float dt);
void EnterStartMenu(void* data, Game& game, int currentState);
void DisplayStartMenu(Console& renderer, const void* gameState);

struct StartMenuData;
extern StartMenuData startMenuData;
