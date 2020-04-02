#include "pch.h"
#include "Common.h"
#include "Instrumentation.h"

void* operator new(size_t size)
{
	Engine2::Instrumentation::MemoryStats.allocated += size;
	return malloc(size);
}

void operator delete(void* memory, size_t size)
{
	Engine2::Instrumentation::MemoryStats.freed += size;
	free(memory);
}


namespace Engine2
{
	void Instrumentation::ImguiWindow(bool* pOpen)
	{
		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
		ImVec4 bg = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg); // store the current background color
		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = { 0,0,0,0 }; // set background to transparent

		if (ImGui::Begin("Instrumentation", pOpen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Memory");
			ImGui::Text("Allocated: %i", MemoryStats.allocated);
			ImGui::Text("Freed    : %i", MemoryStats.freed);
			ImGui::Text("Diff     : %i", MemoryStats.allocated - MemoryStats.freed);

			ImGui::End();
		}

		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = bg; // recover the background color
	}
}
