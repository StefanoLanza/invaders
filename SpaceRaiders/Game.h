// SpaceRaiders.cpp : Defines the entry point for the console application.
//
#include "Vector2D.h"
#include "GameStates.h"
#include "Random.h"
#include <random>


struct GameConfig;
class PlayField;
class MessageLog;
struct AlienWave;
struct WallInfo;
struct BossInfo;
struct Event;


class Game
{
public:

	Game(PlayField& world, const GameConfig& config, std::default_random_engine& rGen, MessageLog& messageLog);

	enum class Mode
	{
		p1,
		cpu1,
		p1p2,
		p1cpu2,
		cpu1cpu2
	};

	int GetScore(int playerIndex) const;
	int GetNumPlayers() const;
	GameStateId GetState() const { return state; }
	void Update(float dt);
	void Start(Mode mode);
	void SetLevel(int level);

public:

	void CreatePlayers(Mode mode);
	void ProcessEvent(const Event& event);
	void SpawnAlienWave(const AlienWave& wave);
	void SpawnWall(const WallInfo& wall);
	void SpawnBoss(const BossInfo& boss);
	void NextLevel();

	static constexpr int maxPlayers = 2;

	PlayField&                  world;
	const GameConfig&           config;
	std::default_random_engine& rGen;
	MessageLog&                 messageLog;

	GameStateId state;
	int   score[maxPlayers];
	int   eventIndex;
	int   wallRow;
	int   numPlayers;
	int   levelIndex;
	float accumTime;
};
