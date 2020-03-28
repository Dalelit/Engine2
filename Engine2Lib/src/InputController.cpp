#include "pch.h"
#include "InputController.h"
#include <Windows.h>

namespace Engine2
{
	void InputController::OnUpdate(float dt)
	{
		float forward = 0.0f, right = 0.0f, up = 0.0f;

		if (IsKeyPressed(KeyboardConfiguration.forward)) forward =  dt * MovementConfiguration.moveSpeed;
		if (IsKeyPressed(KeyboardConfiguration.back))    forward = -dt * MovementConfiguration.moveSpeed;
		if (IsKeyPressed(KeyboardConfiguration.right))   right   =  dt * MovementConfiguration.moveSpeed;
		if (IsKeyPressed(KeyboardConfiguration.left))    right   = -dt * MovementConfiguration.moveSpeed;
		if (IsKeyPressed(KeyboardConfiguration.up))      up      =  dt * MovementConfiguration.moveSpeed;
		if (IsKeyPressed(KeyboardConfiguration.down))    up      = -dt * MovementConfiguration.moveSpeed;
		
		if (forward != 0.0f || right != 0.0f || up != 0.0f) pCamera->Move(forward, right, up);
	}

	void InputController::OnInputEvent(InputEvent& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<MouseMoveEvent>(E2_BIND_EVENT_FUNC(InputController::OnMouseMove));
		dispatcher.Dispatch<MouseButtonPressedEvent>(E2_BIND_EVENT_FUNC(InputController::OnMouseButtonPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(E2_BIND_EVENT_FUNC(InputController::OnMouseButtonReleased));
	}

	void InputController::OnMouseMove(MouseMoveEvent& event)
	{
		if (State.LeftMouseDown) pCamera->Rotate((float)event.GetX() * MovementConfiguration.yawSpeed, (float)event.GetY() * MovementConfiguration.pitchSpeed);
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
				ImGui::Text("Right mouse hold: mouse look");
			}

			if (ImGui::CollapsingHeader("Movement configuration"))
			{
				ImGui::DragFloat("Move speed", &MovementConfiguration.moveSpeed);
				ImGui::DragFloat("Yaw speed", &MovementConfiguration.yawSpeed);
				ImGui::DragFloat("Pitch speed", &MovementConfiguration.pitchSpeed);
			}

			if (ImGui::CollapsingHeader("Input State"))
			{
				if (State.LeftMouseDown) ImGui::Text("Left Mousebutton down"); else  ImGui::Text("Left Mousebutton up");
				if (State.RightMouseDown) ImGui::Text("Right Mousebutton down"); else  ImGui::Text("Right Mousebutton up");
			}
		}
		ImGui::End();
	}

	bool InputController::IsKeyPressed(int vKeyCode)
	{
		return GetKeyState(vKeyCode) & 0x8000;
	}

}