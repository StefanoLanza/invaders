#include "MessageLog.h"
#include <cassert>
#include <cstring>


MessageLog::MessageLog() :
	numMessages { 0 }
{}


void MessageLog::AddMessage(const char* str, Color color)
{
	assert(str);

	// Remove oldest message
	for (int i = maxMessages - 1; i > 0; --i)
	{
		std::memcpy(&messages[i], &messages[i - 1], sizeof(Msg));
	}
	strncpy(messages[0].str, str, sizeof messages[0].str - 1);
	messages[0].t = 0.f;
	messages[0].color = color;
	if (numMessages < maxMessages)
	{
		++numMessages;
	}
}


int MessageLog::GetNumMessages() const
{
	return numMessages;
}


std::pair<const char*, Color> MessageLog::GetMessage(int index) const
{
	return { messages[index].str, messages[index].color };
}


void MessageLog::DeleteOldMessages(float dt, float maxTime)
{
	for (int i = numMessages - 1; i >= 0; --i)
	{
		messages[i].t += dt;
		if (messages[i].t > maxTime)
		{
			--numMessages;
		}
	}
}

void MessageLog::Clear()
{
	numMessages = 0;
}
