#include "pch.h"
#include "InputController.h"
#include "DXDevice.h"
#include <Windows.h>

namespace Engine2
{
	void InputController::OnUpdate(float dt)
	{
		if (!State.WindowFocused) return; // ignore input when window is not focused

		float forward = 0.0f, right = 0.0f, up = 0.0f;

		float speed = dt * MovementConfiguration.moveSpeed;
		if (IsKeyPressed(VK_SHIFT)) speed *= MovementConfiguration.runMultiplier;

		if (IsKeyPressed(KeyboardConfiguration.forward)) forward =  speed;
		if (IsKeyPressed(KeyboardConfiguration.back))    forward = -speed;
		if (IsKeyPressed(KeyboardConfiguration.right))   right   =  speed;
		if (IsKeyPressed(KeyboardConfiguration.left))    right   = -speed;
		if (IsKeyPressed(KeyboardConfiguration.up))      up      =  speed;
		if (IsKeyPressed(KeyboardConfiguration.down))    up      = -speed;
		
		if (forward != 0.0f || right != 0.0f || up != 0.0f) pCamera->Move(forward, right, up);

		pCamera->Update(); // called once per frame for internal calcs

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

		dispatcher.Dispatch<WindowFocusEvent>(E2_BIND_EVENT_FUNC(InputController::OnWindowFocus));
	}

	void InputController::OnMouseMove(MouseMoveEvent& event)
	{
		if (State.LeftMouseDown)  pCamera->Rotate((float)event.GetX() * MovementConfiguration.yawSpeed, (float)event.GetY() * MovementConfiguration.pitchSpeed);
		if (State.RightMouseDown) pCamera->Move(0.0f, (float)event.GetX() * -MovementConfiguration.mouseMoveSpeed, (float)event.GetY() * MovementConfiguration.mouseMoveSpeed);
	}

	void InputController::OnMouseButtonPressed(MouseButtonPressedEvent& event)
	{
		if (event.Left()) State.LeftMouseDown = true;
		if (event.Right()) State.RightMouseDown = true;
	}

	void InputController::OnMouseButtonReleased(MouseButtonReleasedEvent& event)
	{
		if (event.Left()) State.LeftMouseDown = false;
		if (event.Right()) State.RightMouseDown = false;
	}

	void InputController::OnMouseScroll(MouseScrollEvent& event)
	{
		pCamera->Move((float)event.GetDelta() * MovementConfiguration.mouseScrollSpeed, 0.0f, 0.0f);
	}

	void InputController::OnWindowFocus(WindowFocusEvent& event)
	{
		State.WindowFocused = event.IsActive();
	}

	void InputController::ImguiWindow(bool* pOpen)
	{
		if (ImGui::Begin("Input Controller", pOpen))
		{
			if (ImGui::CollapsingHeader("Input configuration"))
			{
				ImGui::Text("Forward: %c", KeyboardConfiguration.forward);
				ImGui::Text("Left   : %c", KeyboardConfiguration.left);
				ImGui::Text("Back   : %c", KeyboardConfiguration.back);
				ImGui::Text("Right  : %c", KeyboardConfiguration.right);
				ImGui::Text("Up     : %c", KeyboardConfiguration.up);
				ImGui::Text("Down   : %c", KeyboardConfiguration.down);
				ImGui::Text("Quit   : ESC");
				ImGui::Text("Run    : Hold Shift");
				ImGui::Text("Right mouse hold: look");
				ImGui::Text("Left  mouse hold: pan");
				ImGui::Text("Mouse scroll: move Forward/Back");
			}

			if (ImGui::CollapsingHeader("Movement configuration"))
			{
				ImGui::DragFloat("Move speed", &MovementConfiguration.moveSpeed);
				ImGui::DragFloat("Mouse move speed", &MovementConfiguration.mouseMoveSpeed);
				ImGui::DragFloat("Mouse scroll speed", &MovementConfiguration.mouseScrollSpeed);
				ImGui::DragFloat("Run multiplier", &MovementConfiguration.runMultiplier);
				ImGui::DragFloat("Yaw speed", &MovementConfiguration.yawSpeed);
				ImGui::DragFloat("Pitch speed", &MovementConfiguration.pitchSpeed);
			}

			if (ImGui::CollapsingHeader("Input State"))
			{
				if (State.LeftMouseDown)  ImGui::Text("Left Mousebutton down"); else  ImGui::Text("Left Mousebutton up");
				if (State.RightMouseDown) ImGui::Text("Right Mousebutton down"); else  ImGui::Text("Right Mousebutton up");
				if (State.WindowFocused)  ImGui::Text("Window active"); else  ImGui::Text("Window inactive");
				ImGui::Text("Mouse client position: (%i, %i)", State.MouseClientPosition.x, State.MouseClientPosition.y);
				ImGui::Text("Mouse client normalised: (%.3f, %.3f)", State.MouseNormaliseCoordinates.x, State.MouseNormaliseCoordinates.y);
				if (State.MouseOffScreen) ImGui::Text("Mouse is off screen"); else ImGui::Text("Mouse is on screen");
			}
		}
		ImGui::End();
	}

	bool InputController::IsKeyPressed(int vKeyCode)
	{
		return GetKeyState(vKeyCode) & 0x8000;
	}
}