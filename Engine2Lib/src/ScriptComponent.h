#pragma once
#include "Script.h"
#include "Entity.h"

namespace Engine2
{
	class ScriptComponent
	{
	public:
		void OnUpdate(float dt) { for (auto& s : scripts) if (s->IsActive()) s->OnUpdate(dt); }
		void OnImgui();

		inline void SetEntity(const Entity& e) { entity = e; }

		template <typename T>
		static void RegisterScript(const std::string& scriptName)
		{
			auto func = [](ScriptVector& scripts) {
				scripts.emplace_back(std::make_unique<T>());
			};
			constructors[scriptName] = func;
		}

		void CreateInstance(const std::string& scriptName);

	protected:
		Entity entity;

		using ScriptVector = std::vector<std::unique_ptr<Script>>;
		ScriptVector scripts;

		static std::map<std::string, void(*)(ScriptVector&)> constructors;
	};
}