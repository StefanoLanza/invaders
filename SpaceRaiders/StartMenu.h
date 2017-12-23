#pragma once


struct Game;
class Renderer;


int StartMenu(Game& game, void* data, float dt);
void EnterStartMenu(void* data, int currentState);
void DisplayStartMenu(Renderer& renderer, const void* gameState);

struct StartMenuData;
extern StartMenuData startMenuData;
