#pragma once

namespace Engine2
{
	class Instrumentation
	{
	public:

		// Memory tracking info
		static struct {
			unsigned long long allocated;
			unsigned long long freed;
		} MemoryStats;

		static void ImguiWindow(bool* pOpen);
	};
}
