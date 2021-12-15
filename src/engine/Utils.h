#pragma once

#include <vector>


namespace Utils
{


// Remove elements from a std::vector in O(1). This method does NOT preserve the order of the vector elements.
// Generally faster than std::remove_if
template <class T, class Pr>
void RemoveElements(std::vector<T>& container, Pr pred)
{
	for (auto it = container.begin(); it != container.end(); )
	{
		if (pred(*it))
		{
			if (it == container.end() - 1)
			{
				// The iterator to the last element is invalidated by pop_back and becomes uncompatible with container.end() afterwards
				// and triggers an assert in VS 2015/2017
				container.pop_back();
				break;
			}
			else
			{
				*it = std::move(container.back());
				container.pop_back();
			}
		}
		else
		{
			++it;
		}
	}
}


}
