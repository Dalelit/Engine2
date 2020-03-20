#include "pch.h"
#include "DXImgui.h"
#include "submodules/Imgui/Imgui.h"
#include "submodules/Imgui/imgui_impl_dx11.h"
#include "submodules/Imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Engine2
{

	LRESULT DXImgui::WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	}

	void DXImgui::Initialise(HWND hwnd, DXDevice& device)
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		//ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(device.GetDevice(), device.GetContext());
	}

	DXImgui::~DXImgui()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void DXImgui::BeginFrame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void DXImgui::Draw()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	bool DXImgui::WantCaptureKeyboard()
	{
		return ImGui::GetIO().WantCaptureKeyboard;
	}
}