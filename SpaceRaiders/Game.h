#include "Vector2D.h"
#include "GameStateMgr.h"
#include <random>


struct GameConfig;
class PlayField;
class MessageLog;
struct AlienWave;
struct WallInfo;
struct BossInfo;
struct Event;
struct ScriptModule;


class Game
{
public:

	Game(PlayField& world, const GameConfig& config, std::default_random_engine& rGen, MessageLog& messageLog, const ScriptModule& scriptModule);

	enum class Mode
	{
		p1,
		cpu1,
		p1p2,
		p1cpu2,
		cpu1cpu2
	};

	int GetScore(int playerIndex) const;
	void Start(Mode mode);

public:

	void CreatePlayers(Mode mode);
	void ProcessEvent(const Event& event);
	void SpawnAlienWave(const AlienWave& wave);
	void SpawnWall(const WallInfo& wall);
	void SpawnBoss(const BossInfo& boss);

	static constexpr int maxPlayers = 2;

	std::vector<GameState>      states;
	PlayField&                  world;
	const GameConfig&           config;
	std::default_random_engine& rGen;
	MessageLog&                 messageLog;
	const ScriptModule&         scriptModule;
	int   stateId;
	int   score[maxPlayers];
	int   eventIndex;
	int   numPlayers;
};
