#pragma once

#include "pch.h"
#include "Common.h"
#include "Events.h"
#include "Camera.h"

namespace Engine2
{
	class InputController
	{
	public:
		InputController(Camera* pCamera) : pCamera(pCamera) {}
		~InputController() = default;

		struct {
			DWORD forward = 'W';
			DWORD back    = 'S';
			DWORD right   = 'D';
			DWORD left    = 'A';

			DWORD quitKey = VK_ESCAPE;
		} KeyboardConfiguration;

		struct {
			float moveSpeed = 1.0f;
			float yawSpeed = 0.001f;
			float pitchSpeed = 0.001f;
		} MovementConfiguration;

		struct {
			bool LeftMouseDown = false;
			bool RightMouseDown = false;
		} State;

		void OnUpdate(float dt);
		void OnInputEvent(InputEvent& event);
		void ImguiWindow(bool* pOpen);

		bool IsKeyPressed(int vKeyCode);

	protected:
		Camera* pCamera;

		void OnMouseMove(MouseMoveEvent& event);
		void OnMouseButtonPressed(MouseButtonPressedEvent& event);
		void OnMouseButtonReleased(MouseButtonReleasedEvent& event);
	};
}