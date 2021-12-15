#pragma once

#include "Colors.h"
#include <utility> // pair


class MessageLog
{
public:

	MessageLog();

	void AddMessage(const char* str, Color color = Color::yellowIntense);
	int GetNumMessages() const;
	std::pair<const char*, Color> GetMessage(int index) const;
	void DeleteOldMessages(float dt, float maxTime);
	void Clear();

private:

	static constexpr int maxMessages = 2;

	struct Msg
	{
		char  str[64];
		Color color;
		float t;
	};

	Msg   messages[maxMessages];
	int   numMessages;
};
