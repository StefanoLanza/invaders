#pragma once


class Game;
class Renderer;


int StartMenu(Game& game, void* data, float dt);
void DisplayStartMenu(Renderer& renderer, const void* gameState);
