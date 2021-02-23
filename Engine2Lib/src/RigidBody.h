#pragma once
#include "Components.h"
#include "Transform.h"
#include "Serialiser.h"

namespace Engine2
{
	class RigidBody
	{
	public:
		void OnUpdate(float dt, Transform* pTransform);

		inline bool IsKinematic() const { return isKinematic; }
		inline bool UseGravity() const { return useGravity; }

		void OnImgui();

		void Serialise(Serialisation::INode& node);

	protected:
		float mass = 1.0f;
		bool isKinematic = false;
		bool useGravity = true;
	};


}