#pragma once

#include "pch.h"
#include "Common.h"
#include "Events.h"
#include "DXDevice.h"

namespace Engine2
{
	class InputController
	{
	public:
		InputController();

		struct InputState {
			bool LeftMouseDown  = false;
			bool RightMouseDown = false;
			bool MiddleMouseDown = false;
			bool WindowFocused  = true;
			bool ClipCursor = false; // note: call method rather than update directly
			RECT clientRect; // client rect in screen coordinates
			struct { int x, y; } MouseMove;
			int MouseScroll;
			struct { UINT32 x, y; } MouseClientPosition;
			struct { float x, y; }  MouseNormaliseCoordinates;
			bool MouseOffScreen;

			inline bool IsKeyPressed(int vKeyCode) const { return GetKeyState(vKeyCode) & 0x8000; }
			inline bool HasMouseMoved() const { return MouseMove.x != 0 || MouseMove.y != 0; }

		} State;

		void FrameReset(); // initialise variables dependent on the frame

		void OnUpdate(float dt);
		void OnInputEvent(InputEvent& event);
		void OnApplicationEvent(ApplicationEvent& event);
		void ImguiWindow(bool* pOpen);

		void SetCursorClipping(bool clipped) { State.ClipCursor = clipped; UpdateCursorClipping(); }

	protected:

		void OnMouseMove(MouseMoveEvent& event);
		void OnMouseButtonPressed(MouseButtonPressedEvent& event);
		void OnMouseButtonReleased(MouseButtonReleasedEvent& event);
		void OnMouseScroll(MouseScrollEvent& event);
		void OnWindowMove(WindowMoveEvent& event);
		void OnWindowResize(WindowResizeEvent& event);
		void OnWindowFocus(WindowFocusEvent& event);

		void UpdateCursorClipping();
		void UpdateRects();
	};
}