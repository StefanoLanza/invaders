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


struct Game
{
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

	static constexpr int maxPlayers = 2;

	const GameConfig&           config;
	std::default_random_engine& rGen;
	MessageLog&                 messageLog;
	const ScriptModule&         scriptModule;
	PlayField&                  world;
	std::vector<GameState>      states;
	int   stateId;
	int   score[maxPlayers];
	int   numPlayers;
};


Game NewGame(PlayField& world, const GameConfig& config, std::default_random_engine& rGen, MessageLog& messageLog, const ScriptModule& scriptModule);
