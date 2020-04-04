#pragma once

namespace Engine2
{
	namespace Instrumentation
	{
		class Memory
		{
		public:
			static void ImguiWindow(bool* pOpen);

			static unsigned long long allocated;
			static unsigned long long freed;
		};

		template <typename T, unsigned int SIZE>
		class AverageTracker
		{
		public:
			void Log(T value)
			{
				current++;                        // next
				if (current >= SIZE) current = 0; // loop around array
				data[current] = value;            // set the value
			}

			float Average()
			{
				T amt = 0;
				for (auto v : data)
				{
					amt += v;
				}
				return (float)amt / (float)SIZE;
			}

			unsigned int current = 0;
			unsigned int size = SIZE;
			T data[SIZE];
		};

		class Timer
		{
		public:
			Timer() { startTime = clock(); }
			inline void Set() { startTime = clock(); }

			inline void Tick() {
				clock_t current = clock();
				times.Log((float)(current - startTime));
				startTime = current;
			}

			inline float Average() { return times.Average(); }

			void OnImgui();

		protected:
			clock_t startTime;
			AverageTracker<float, 60> times;
		};

		class MemoryTracker
		{
		public:
			MemoryTracker() { Set(); }
			void Set() { allocated = Memory::allocated; freed = Memory::freed; }
			void Tick() {
				allocations.Log(Memory::allocated - allocated);
				freeds.Log(Memory::freed - freed);
				Set();
			}

			AverageTracker<unsigned long long, 60> allocations;
			AverageTracker<unsigned long long, 60> freeds;

		protected:
			unsigned long long allocated;
			unsigned long long freed;
		};
	}
}
