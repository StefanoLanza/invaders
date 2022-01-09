#include <engine/Vector2D.h>
#include <random>

struct GameConfig;
class PlayField;
class MessageLog;
struct AlienWave;
struct WallInfo;
struct BossInfo;
struct Event;
struct AIModule;
struct GameState;

struct Game
{
	enum class Mode
	{
		p1,
		p1p2,
	};

	static constexpr int maxPlayers = 2;

	const GameConfig &config;
	std::default_random_engine &rGen;
	MessageLog &messageLog;
	const AIModule &scriptModule;
	PlayField &world;
	std::vector<GameState> states;
	Mode mode;
	int stateId;
	int score[maxPlayers];
	int numPlayers;
};

Game NewGame(PlayField &world, const GameConfig &config, std::default_random_engine &rGen, MessageLog &messageLog, const AIModule &scriptModule);
