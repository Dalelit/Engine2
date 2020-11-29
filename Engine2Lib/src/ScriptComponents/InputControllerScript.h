#pragma once

#include "Script.h"

namespace Engine2
{
	class InputControllerScript : public Script
	{
	public:
		void OnUpdate(float dt);
		void OnImgui();
		bool IsValid();

		struct {
			DWORD forward = 'W';
			DWORD back = 'S';
			DWORD right = 'D';
			DWORD left = 'A';
			DWORD up = 'E';
			DWORD down = 'Q';

			DWORD quit = VK_ESCAPE;
		} KeyboardConfiguration;

		struct {
			bool  mouseLook = false;
			float moveSpeed = 1.0f;
			float mouseMoveSpeedX = -0.01f;
			float mouseMoveSpeedY = -0.01f;
			float mouseScrollSpeed = 0.5f;
			float runMultiplier = 3.0f;
			float yawSpeed = 0.001f;
			float pitchSpeed = -0.001f;
		} MovementConfiguration;
	};
}