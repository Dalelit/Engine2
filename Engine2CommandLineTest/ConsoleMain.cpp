// Commandline program to quickly test stuff.

#include <iostream>
#include "ECS.h"
#include <DirectXMath.h>
#include <vector>

using namespace EngineECS;

struct Transform
{
	DirectX::XMVECTOR position;

	Transform()
	{
		position.m128_f32[0] = 0.1f;
		position.m128_f32[1] = 1.0f;
		position.m128_f32[2] = 2.0f;
		position.m128_f32[3] = 3.0f;
	}

	friend std::ostream& operator<<(std::ostream& out, Transform* t)
	{
		if (t)
			out << t->position.m128_f32[0] << "," << t->position.m128_f32[1] << "," << t->position.m128_f32[2] << "," << t->position.m128_f32[3];
		return out;
	}

	friend std::ostream& operator<<(std::ostream& out, Transform& t)
	{
		out << t.position.m128_f32[0] << "," << t.position.m128_f32[1] << "," << t.position.m128_f32[2] << "," << t.position.m128_f32[3];
		return out;
	}
};

class RigidBody
{
public:
	DirectX::XMVECTOR velocity;
};

class Mesh
{
public:
private:
	void* data = nullptr;
};


int main()
{
	std::cout << "Starting..." << std::endl;
	std::cout << "Max entities " << MAXENTITIES << std::endl;
	std::cout << "Max components " << (uint32_t)MAXCOMPONENTS << std::endl;
	std::cout << "---------" << std::endl;

	Coordinator coord;
	std::cout << coord << std::endl;
	std::cout << "---------" << std::endl;

	EntityId_t e1 = coord.CreateEntity();
	coord.AddComponent<Transform>(e1);

	EntityId_t e2 = coord.CreateEntity();
	coord.AddComponent<Transform>(e2);
	coord.AddComponent<RigidBody>(e2);

	std::cout << "e2 has RigidBody " << coord.HasComponent<RigidBody>(e2) << std::endl;
	std::cout << "e2 has Mesh " << coord.HasComponent<Mesh>(e2) << std::endl;

	for (int i = 0; i < 15; i++)
	{
		EntityId_t eid = coord.CreateEntity();

		if (i > 5) coord.AddComponent<Transform>(eid);
		if (i > 10) coord.AddComponent<RigidBody>(eid);
		if (i > 13) coord.AddComponent<Mesh>(eid);
	}

	coord.AddComponent<RigidBody>(coord.CreateEntity());

	std::cout << coord << std::endl;

	std::cout << "---------" << std::endl;
	coord.GetComponent<Transform>(e2)->position.m128_f32[0] = 666.0f;
	std::cout << coord.GetComponent<Transform>(e1) << std::endl;
	std::cout << coord.GetComponent<Transform>(e2) << std::endl;
	std::cout << "---------" << std::endl;

	{
		EntityView<Transform> v(coord);
		std::cout << v << std::endl;
		for (auto t : v) std::cout << " " << t << std::endl;
		std::cout << "---------" << std::endl;
	}
	{
		View<Transform, RigidBody> v(coord);
		std::cout << v << std::endl;
		for (auto t : v) std::cout << " " << t << std::endl;
		std::cout << "---------" << std::endl;
	}
	{
		EntityView<RigidBody> v(coord);
		std::cout << v << std::endl;
		for (auto t : v) std::cout << " " << t << std::endl;
		std::cout << "---------" << std::endl;
	}
	{
		EntityView v(coord);
		std::cout << v << std::endl;
		for (auto t : v) std::cout << " " << t << std::endl;
		std::cout << "---------" << std::endl;
	}

	{
		std::cout << coord.GetComponents<Transform>() << std::endl;
		for (auto& c : coord.GetComponents<Transform>()) std::cout << " " << c << std::endl;
		std::cout << "---------" << std::endl;
	}

	{
		View<RigidBody> v(coord);
		std::cout << v << std::endl;
		for (auto t : v) std::cout << " " << t << std::endl;
		std::cout << "---------" << std::endl;
	}

	{
		EntityView<Mesh> v(coord);
		std::cout << v << std::endl;
		for (auto t : v) std::cout << " " << t << std::endl;
		std::cout << "---------" << std::endl;
	}
	{
		View<Mesh> v(coord);
		std::cout << v << std::endl;
		for (auto t : v) std::cout << " " << t << std::endl;
		std::cout << "---------" << std::endl;
	}

	std::cout << "Done." << std::endl;
	return 0;
}
