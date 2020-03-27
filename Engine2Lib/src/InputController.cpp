#include "pch.h"
#include "InputController.h"
#include <Windows.h>

namespace Engine2
{
	void InputController::OnUpdate(float dt)
	{
		if (IsKeyPressed(KeyboardConfiguration.forward)) pCamera->TranslateForward(dt * MovementConfiguration.moveSpeed);
		if (IsKeyPressed(KeyboardConfiguration.back))    pCamera->TranslateForward(-dt * MovementConfiguration.moveSpeed);
		if (IsKeyPressed(KeyboardConfiguration.right))   pCamera->TranslateRight(dt * MovementConfiguration.moveSpeed);
		if (IsKeyPressed(KeyboardConfiguration.left))    pCamera->TranslateRight(-dt * MovementConfiguration.moveSpeed);
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
			if (ImGui::CollapsingHeader("Keyboard configuration"))
			{

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