enum class EventType
{
	wait,
	message,
	spawnWave,
	spawnWalls,
	boss,
};


struct Event
{
	EventType   type;
	float       time;
	const void* data;
};
