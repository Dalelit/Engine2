#pragma once

#define E2_STATS_VERTEXDRAW(x)                 { Instrumentation::Drawing::drawCount++; Instrumentation::Drawing::vertexCount += x; }
#define E2_STATS_INDEXDRAW(x)                  { Instrumentation::Drawing::drawCount++; Instrumentation::Drawing::indexCount += x; }
#define E2_STATS_INDEXINSTANCEDRAW(indx, inst) { Instrumentation::Drawing::drawCount++; Instrumentation::Drawing::indexCount += indx; Instrumentation::Drawing::instanceCount += inst; }


#define E2_STATS_VSCB_BIND     { Instrumentation::Drawing::vsConstBufferCount++; }
#define E2_STATS_PSCB_BIND     { Instrumentation::Drawing::psConstBufferCount++; }
#define E2_STATS_GSCB_BIND     { Instrumentation::Drawing::gsConstBufferCount++; }

namespace Engine2
{
	namespace Instrumentation
	{
		void FrameReset();

		class Memory
		{
		public:
			static void ImguiWindow(bool* pOpen);

			static unsigned long long allocated;
			static unsigned long long freed;
		};

		class Drawing
		{
		public:
			static void ImguiWindow(bool* pOpen);

			static unsigned long long drawCount;
			static unsigned long long vertexCount;
			static unsigned long long indexCount;
			static unsigned long long instanceCount;
			static unsigned long long vsConstBufferCount;
			static unsigned long long psConstBufferCount;
			static unsigned long long gsConstBufferCount;
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
