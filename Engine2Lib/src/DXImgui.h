#pragma once
#include "pch.h"
#include "DXDevice.h"

namespace Engine2
{

	class DXImgui
	{
	public:
		static LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		~DXImgui();

		void Initialise(HWND hwnd, DXDevice& device);

		void BeginFrame();
		void Draw();

		bool WantCaptureKeyboard();

	};

}

