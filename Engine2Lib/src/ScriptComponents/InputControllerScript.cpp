#include "pch.h"
#include "InputControllerScript.h"
#include "Camera.h"
#include "Transform.h"
#include "submodules/imgui/imgui.h"
#include "Engine2.h"

namespace Engine2
{
	bool InputControllerScript::IsValid()
	{
		return entity.HasComponent<Transform>();
	}

	void InputControllerScript::Serialise(Serialisation::INode& node)
	{
		node.Attribute("mouseLook", MovementConfiguration.mouseLook);
		node.Attribute("moveSpeed", MovementConfiguration.moveSpeed);
		node.Attribute("mouseMoveSpeedX", MovementConfiguration.mouseMoveSpeedX);
		node.Attribute("mouseMoveSpeedY", MovementConfiguration.mouseMoveSpeedY);
		node.Attribute("mouseScrollSpeed", MovementConfiguration.mouseScrollSpeed);
		node.Attribute("runMultiplier", MovementConfiguration.runMultiplier);
		node.Attribute("yawSpeed", MovementConfiguration.yawSpeed);
		node.Attribute("pitchSpeed", MovementConfiguration.pitchSpeed);
	}

	void InputControllerScript::OnUpdate(float dt)
	{
		auto& inputState = Engine::Get().InputState();
		
		if (!inputState.WindowFocused) return; // ignore input when window is not focused

		float forward = 0.0f, right = 0.0f, up = 0.0f;
		float speed = dt * MovementConfiguration.moveSpeed;

		auto pTransform = entity.GetComponent<Transform>();

		if (inputState.IsKeyPressed(VK_SHIFT)) speed *= MovementConfiguration.runMultiplier;
		if (inputState.IsKeyPressed(KeyboardConfiguration.forward)) forward += speed;
		if (inputState.IsKeyPressed(KeyboardConfiguration.back))    forward -= speed;
		if (inputState.IsKeyPressed(KeyboardConfiguration.right))   right += speed;
		if (inputState.IsKeyPressed(KeyboardConfiguration.left))    right -= speed;
		if (inputState.IsKeyPressed(KeyboardConfiguration.up))      up += speed;
		if (inputState.IsKeyPressed(KeyboardConfiguration.down))    up -= speed;

		forward += (float)inputState.MouseScroll * MovementConfiguration.mouseScrollSpeed;

		if (inputState.HasMouseMoved())
		{
			if (MovementConfiguration.mouseLook || inputState.LeftMouseDown)
				pTransform->Rotate((float)inputState.MouseMove.y * MovementConfiguration.pitchSpeed,
								   (float)inputState.MouseMove.x * MovementConfiguration.yawSpeed,
								   0.0f);
			
			if (inputState.MiddleMouseDown)
			{
				right += inputState.MouseMove.x * MovementConfiguration.mouseMoveSpeedX;
				up += inputState.MouseMove.y * MovementConfiguration.mouseMoveSpeedY;
			}
		}

		pTransform->Move(forward, right, up);
	}

	void InputControllerScript::OnImgui()
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
			if (!MovementConfiguration.mouseLook) ImGui::Text("Right mouse hold: look");
			ImGui::Text("Middle mouse hold: pan");
			ImGui::Text("Mouse scroll: move Forward/Back");
		}

		if (ImGui::CollapsingHeader("Movement configuration"))
		{
			ImGui::Checkbox("Mouse look", &MovementConfiguration.mouseLook);
			ImGui::DragFloat("Move speed", &MovementConfiguration.moveSpeed);
			ImGui::DragFloat("Mouse move speed x", &MovementConfiguration.mouseMoveSpeedX);
			ImGui::DragFloat("Mouse move speed y", &MovementConfiguration.mouseMoveSpeedY);
			ImGui::DragFloat("Mouse scroll speed", &MovementConfiguration.mouseScrollSpeed);
			ImGui::DragFloat("Run multiplier", &MovementConfiguration.runMultiplier);
			ImGui::DragFloat("Yaw speed", &MovementConfiguration.yawSpeed);
			ImGui::DragFloat("Pitch speed", &MovementConfiguration.pitchSpeed);
		}
	}

}