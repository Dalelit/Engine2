#include "pch.h"
#include "InputController.h"
#include <Windows.h>

namespace Engine2
{

	InputController::InputController()
	{
		UpdateRects();
		FrameReset();
	}

	void InputController::FrameReset()
	{
		State.MouseMove.x = 0;
		State.MouseMove.y = 0;
		State.MouseScroll = 0;
	}

	void InputController::OnUpdate(float dt)
	{
		if (!State.WindowFocused) return; // ignore input when window is not focused

		POINT mpos;
		GetCursorPos(&mpos);
		State.MouseOffScreen = DXDevice::Get().ScreenToClientClamped(mpos);
		State.MouseClientPosition.x = mpos.x;
		State.MouseClientPosition.y = mpos.y;
		DXDevice::Get().NormaliseCoordinates(mpos, State.MouseNormaliseCoordinates.x, State.MouseNormaliseCoordinates.y);
	}

	void InputController::OnInputEvent(InputEvent& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<MouseMoveEvent>(E2_BIND_EVENT_FUNC(InputController::OnMouseMove));
		dispatcher.Dispatch<MouseButtonPressedEvent>(E2_BIND_EVENT_FUNC(InputController::OnMouseButtonPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(E2_BIND_EVENT_FUNC(InputController::OnMouseButtonReleased));
		dispatcher.Dispatch<MouseScrollEvent>(E2_BIND_EVENT_FUNC(InputController::OnMouseScroll));
	}

	void InputController::OnApplicationEvent(ApplicationEvent& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowMoveEvent>(E2_BIND_EVENT_FUNC(InputController::OnWindowMove));
		dispatcher.Dispatch<WindowResizeEvent>(E2_BIND_EVENT_FUNC(InputController::OnWindowResize));
		dispatcher.Dispatch<WindowFocusEvent>(E2_BIND_EVENT_FUNC(InputController::OnWindowFocus));
	}

	void InputController::OnMouseMove(MouseMoveEvent& event)
	{
		State.MouseMove.x += event.GetX();
		State.MouseMove.y += event.GetY();
	}

	void InputController::OnMouseButtonPressed(MouseButtonPressedEvent& event)
	{
		if (event.Left()) State.LeftMouseDown = true;
		if (event.Right()) State.RightMouseDown = true;
		if (event.Middle()) State.MiddleMouseDown = true;
	}

	void InputController::OnMouseButtonReleased(MouseButtonReleasedEvent& event)
	{
		if (event.Left()) State.LeftMouseDown = false;
		if (event.Right()) State.RightMouseDown = false;
		if (event.Middle()) State.MiddleMouseDown = false;
	}

	void InputController::OnMouseScroll(MouseScrollEvent& event)
	{
		State.MouseScroll += event.GetDelta();
	}

	void InputController::OnWindowMove(WindowMoveEvent& event)
	{
		UpdateRects();
		UpdateCursorClipping();
	}

	void InputController::OnWindowResize(WindowResizeEvent& event)
	{
		UpdateRects();
		UpdateCursorClipping();
	}

	void InputController::OnWindowFocus(WindowFocusEvent& event)
	{
		State.WindowFocused = event.IsActive();
	}

	// to do : need to support window move and size change to update the clip rect
	// to do : need to fix the clip rect going over the title bar
	void InputController::UpdateCursorClipping()
	{
		if (State.ClipCursor)
		{
			ClipCursor(&State.clientRect);
		}
		else
		{
			ClipCursor(nullptr);
		}
	}

	void InputController::UpdateRects()
	{
		HWND hwnd = DXDevice::Get().GetWindowHandle();
		GetClientRect(hwnd, &State.clientRect);

		ClientToScreen(hwnd, reinterpret_cast<POINT*>(&State.clientRect.left)); // convert top-left
		ClientToScreen(hwnd, reinterpret_cast<POINT*>(&State.clientRect.right)); // convert bottom-right
	}

	void InputController::ImguiWindow(bool* pOpen)
	{
		if (ImGui::Begin("Input Controller State", pOpen))
		{
			if (ImGui::Checkbox("Clip cursor", &State.ClipCursor)) UpdateCursorClipping();
			ImGui::Text("Screen client rect (%i,%i),(%i,%i)", State.clientRect.left, State.clientRect.top, State.clientRect.right, State.clientRect.bottom);
			ImGui::Text("Mouse move %i,%i", State.MouseMove.x, State.MouseMove.y);
			ImGui::Text("Mouse scroll %i", State.MouseScroll);
			if (State.LeftMouseDown)   ImGui::Text("Left Mousebutton down"); else  ImGui::Text("Left Mousebutton up");
			if (State.RightMouseDown)  ImGui::Text("Right Mousebutton down"); else  ImGui::Text("Right Mousebutton up");
			if (State.MiddleMouseDown) ImGui::Text("Middle Mousebutton down"); else  ImGui::Text("Middle Mousebutton up");
			if (State.WindowFocused)   ImGui::Text("Window focused"); else  ImGui::Text("Window not focused");
			ImGui::Text("Mouse client position: (%i, %i)", State.MouseClientPosition.x, State.MouseClientPosition.y);
			ImGui::Text("Mouse client normalised: (%.3f, %.3f)", State.MouseNormaliseCoordinates.x, State.MouseNormaliseCoordinates.y);
			if (State.MouseOffScreen) ImGui::Text("Mouse is off screen"); else ImGui::Text("Mouse is on screen");
		}
		ImGui::End();
	}

}