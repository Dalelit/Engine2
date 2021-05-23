#include "pch.h"
#include "UID.h"
#include <time.h>

namespace Engine2
{
	UIDGenerator::UIDGenerator()
	{
		time_t t;
		time(&t);
		start = (uint32_t)t; // only take the lower half of the 64 bit time
	}

	UID UIDGenerator::Next()
	{
		union {
			uint64_t id;
			uint32_t half[2];
		} value;

		value.half[0] = count++;
		value.half[1] = start;

		return UID(value.id);
	}
}