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
		unsigned long long Memory::allocated = 0;
		unsigned long long Memory::freed = 0;
		unsigned long long Drawing::drawCount = 0;
		unsigned long long Drawing::vertexCount = 0;
		unsigned long long Drawing::indexCount = 0;
		unsigned long long Drawing::vsConstBufferCount = 0;
		unsigned long long Drawing::psConstBufferCount = 0;
		unsigned long long Drawing::gsConstBufferCount = 0;

		void FrameReset()
		{
			Drawing::drawCount = 0;
			Drawing::vertexCount = 0;
			Drawing::indexCount = 0;
			Drawing::vsConstBufferCount = 0;
			Drawing::psConstBufferCount = 0;
			Drawing::gsConstBufferCount = 0;
		}

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

		void Drawing::ImguiWindow(bool* pOpen)
		{
			ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
			ImVec4 bg = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg); // store the current background color
			ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = { 0,0,0,0 }; // set background to transparent

			if (ImGui::Begin("Drawing counters", pOpen, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Draw  : %i", drawCount);
				ImGui::Text("Verticies : %i\n", vertexCount + indexCount);
				ImGui::Text("   Vertex : %i", vertexCount);
				ImGui::Text("   Index  : %i", indexCount);
				ImGui::Text("Const Buff Bind: %i", vsConstBufferCount + psConstBufferCount + gsConstBufferCount);
				ImGui::Text("   VS : %i", vsConstBufferCount);
				ImGui::Text("   PS : %i", psConstBufferCount);
				ImGui::Text("   GS : %i", gsConstBufferCount);

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
