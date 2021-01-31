#pragma once
#include "Components.h"
#include "Transform.h"

namespace Engine2
{
	class RigidBody
	{
	public:
		void OnUpdate(float dt, Transform* pTransform);

		void OnImgui();

	protected:
		float mass = 1.0f;
		bool isKinematic = false;
		bool useGravity = true;
	};


}