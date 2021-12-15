#include <engine/RenderItem.h>


struct AlienPrefab
{
	Animation anim;
	Color     color;
	int       health;
	float     speed;
};


struct PlayerPrefab
{
	ImageId imageId;
	Color   color;
	Color   invulnColor;
	float   velocity;
	float   laserOffset;
};

const AlienPrefab& GetAlienPrefab(int index);
const AlienPrefab& GetBossPrefab(int index);
const PlayerPrefab& GetPlayerPrefab(int index);
