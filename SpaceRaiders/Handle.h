#pragma once

#include <stdint.h>


class Handle
{
public:

	constexpr Handle() : value {0} {}
	constexpr explicit Handle(uint32_t value) : value {value} {}
	constexpr Handle(uint32_t index_, uint32_t generation_) : index {index_}, generation {generation_} {}
	void Set(uint32_t index_, uint32_t generation_) { index = index_; generation = generation_; }
	explicit operator bool() const { return value != 0; }
	void Release() { value = 0; }

	union
	{
		struct
		{
			uint32_t index  : 24;
			uint32_t generation   : 8;
		};
		uint32_t value;
	};
};


static constexpr Handle nullHandle;


inline bool operator == (const Handle& a, const Handle& b) { return a.value == b.value; }
inline bool operator != (const Handle& a, const Handle& b) { return a.value != b.value; }


template <class T>
class TypedHandle : public Handle {};