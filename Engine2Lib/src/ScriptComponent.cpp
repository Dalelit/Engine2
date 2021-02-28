#include "pch.h"
#include "ScriptComponent.h"
#include "submodules/imgui/imgui.h"

namespace Engine2
{
	std::map<std::string, void(*)(ScriptComponent::ScriptVector&)> ScriptComponent::constructors;

	void ScriptComponent::OnImgui()
	{
		int deleteIndex = -1;
		for (int i = 0; i < scripts.size(); ++i)
		{
			auto& s = scripts[i];
			if (ImGui::TreeNodeEx(s->Name().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				bool active = s->IsActive();
				if (ImGui::Checkbox("Active", &active)) { if (active) s->SetActive(); else s->SetInactive(); }
				scripts[i]->OnImgui();
				ImGui::TreePop();
			}

			if (ImGui::BeginPopupContextItem(scripts[i]->Name().c_str()))
			{
				if (ImGui::MenuItem("Delete script")) deleteIndex = i;
				ImGui::EndPopup();
			}
		}
		if (deleteIndex >= 0) scripts.erase(scripts.begin() + deleteIndex);

		if (ImGui::BeginCombo("Add Script", ""))
		{
			for (auto& [k, v] : constructors)
			{
				// Determine if it already has this script
				// To do: is there a nicer way?
				bool hasScript = false;
				for (auto i = 0; !hasScript && i < scripts.size(); ++i) if (k == scripts[i]->Name()) hasScript = true;
				
				if (!hasScript && ImGui::Selectable(k.c_str()))
				{
					v(scripts);
					auto& s = scripts.back();
					s->SetEntity(entity);
					s->SetName(k);
					s->OnInitialise();
					s->SetActive();
				}
			}
			ImGui::EndCombo();
		}
	}

	std::shared_ptr<Script> ScriptComponent::CreateInstance(const std::string& scriptName)
	{
		auto func = constructors[scriptName];
		if (func) func(scripts);
		auto& s = scripts.back();
		s->SetName(scriptName);
		s->SetEntity(entity);
		s->OnInitialise();

		return s;
	}
}