// FIXME In game, not engine
enum class EventType
{
	wait,
	message,
	spawnWave,
	boss,
};


struct Event
{
	EventType   type;
	float       time;
	const void* data;
};
