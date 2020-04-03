#include "pch.h"
#include "Common.h"
#include "Instrumentation.h"


void* operator new(size_t size)
{
	Engine2::Instrumentation::Memory::allocated += size;
	return malloc(size);
}

void operator delete(void* memory, size_t size)
{
	Engine2::Instrumentation::Memory::freed += size;
	free(memory);
}

namespace Engine2
{
	namespace Instrumentation
	{
		unsigned long long Memory::allocated;
		unsigned long long Memory::freed;

		void Memory::ImguiWindow(bool* pOpen)
		{
			ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
			ImVec4 bg = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg); // store the current background color
			ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = { 0,0,0,0 }; // set background to transparent

			if (ImGui::Begin("Memory", pOpen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Allocated: %i", allocated);
				ImGui::Text("Freed    : %i", freed);
				ImGui::Text("Diff     : %i", allocated - freed);

				ImGui::End();
			}

			ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = bg; // recover the background color
		}

		void Timer::OnImgui()
		{
			char buffer[13];
			sprintf_s(buffer, ARRAYSIZE(buffer), "ms %.1f", times.Average());
			ImGui::PlotLines("", times.data, times.size, times.current, buffer, 0.0f, 30.0f, { 0,40 });
		}
	}
}
