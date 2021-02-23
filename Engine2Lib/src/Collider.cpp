#include "pch.h"
#include "Collider.h"
#include "submodules/imgui/imgui.h"

namespace Engine2
{
	Collider::Collider()
	{
		InitialiseAsBox();
	}

	void Collider::OnImgui()
	{
		// display the collider type as a combo box, which means it can also be changed.
		const char* selected = "";
		switch (type)
		{
		case ColliderType::sphere:  selected = "Sphere"; break;
		case ColliderType::box:     selected = "Box"; break;
		case ColliderType::capsule: selected = "Capsule"; break;
		case ColliderType::plane:   selected = "Plane"; break;
		default:
			ImGui::Text("Invalid collider type!");
			return;
		}

		if (ImGui::BeginCombo("Type", selected))
		{
			if (ImGui::Selectable("Sphere", type == ColliderType::sphere))   InitialiseAsSphere();
			if (ImGui::Selectable("Box", type == ColliderType::box))         InitialiseAsBox();
			//if (ImGui::Selectable("Capsule", type == ColliderType::capsule)) InitialiseAsCapsule();
			//if (ImGui::Selectable("Plane", type == ColliderType::plane))     InitialiseAsPlane();
			ImGui::EndCombo();
		}

		ImGui::Checkbox("Show collider", &showCollider);
		ImGui::DragFloat3("Centre", &centre.x, 0.1f);
		
		switch (type)
		{
		case ColliderType::sphere:
			ImGui::DragFloat("Radius", &extents.x, 0.1f);
			break;
		case ColliderType::box:
			ImGui::DragFloat3("Extents", &extents.x, 0.1f);
			ImGui::Text("Axis up (Y)");
			break;
		case ColliderType::capsule:
			ImGui::DragFloat("Radius", &extents.x, 0.1f);
			ImGui::DragFloat("Half height", &extents.y, 0.1f);
			ImGui::Text("Not imnplemented yet!!");
			break;
		case ColliderType::plane:
			ImGui::Text("Axis up (Y)");
			ImGui::Text("Not imnplemented yet!!");
			break;
		}
	}

	void Collider::Serialise(Serialisation::INode& node)
	{
		node.Attribute("type", (int&)type);
		node.Attribute("showCollider", showCollider);
		node.Attribute("centre", centre);
		node.Attribute("extents", extents);
	}

	void Collider::InitialiseAsSphere()
	{
		type = ColliderType::sphere;
		SetRadius(1.0f);
	}

	void Collider::InitialiseAsBox()
	{
		type = ColliderType::box;
		SetHalfExtents(1.0f);
	}

	void Collider::InitialiseAsCapsule()
	{
		type = ColliderType::capsule;
		SetRadius(1.0f);
		SetHalfHeight(1.0f);
	}

	void Collider::InitialiseAsPlane()
	{
		type = ColliderType::plane;
	}
}