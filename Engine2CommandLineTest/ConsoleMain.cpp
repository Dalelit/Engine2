// Commandline program to quickly test stuff.

#include <iostream>
#include "ECS.h"
#include <DirectXMath.h>
#include <vector>

using namespace EngineECS;

struct TransformTest
{
	DirectX::XMVECTOR position;

	TransformTest()
	{
		position.m128_f32[0] = 0.1f;
		position.m128_f32[1] = 1.0f;
		position.m128_f32[2] = 2.0f;
		position.m128_f32[3] = 3.0f;
	}

	friend std::ostream& operator<<(std::ostream& out, TransformTest* t)
	{
		if (t)
			out << t->position.m128_f32[0] << "," << t->position.m128_f32[1] << "," << t->position.m128_f32[2] << "," << t->position.m128_f32[3];
		return out;
	}

	friend std::ostream& operator<<(std::ostream& out, TransformTest& t)
	{
		out << t.position.m128_f32[0] << "," << t.position.m128_f32[1] << "," << t.position.m128_f32[2] << "," << t.position.m128_f32[3];
		return out;
	}
};

class RigidBodyTest
{
public:
	DirectX::XMVECTOR velocity;
};

class MeshTest
{
public:
private:
	void* data = nullptr;
};


int main()
{
	std::cout << "Starting..." << std::endl;

	Coordinator coord;
	std::cout << coord << std::endl;
	std::cout << "---------" << std::endl;

	EntityId_t e1 = coord.CreateEntity();
	coord.AddComponent<TransformTest>(e1);

	EntityId_t e2 = coord.CreateEntity();
	coord.AddComponent<TransformTest>(e2);
	coord.AddComponent<RigidBodyTest>(e2);

	std::cout << "e2 has RigidBodyTest " << coord.HasComponent<RigidBodyTest>(e2) << std::endl;
	std::cout << "e2 has MeshTest " << coord.HasComponent<MeshTest>(e2) << std::endl;

	for (int i = 0; i < 15; i++)
	{
		EntityId_t eid = coord.CreateEntity();

		if (i > 5) coord.AddComponent<TransformTest>(eid);
		if (i > 10) coord.AddComponent<RigidBodyTest>(eid);
		if (i > 13) coord.AddComponent<MeshTest>(eid);
	}

	coord.AddComponent<RigidBodyTest>(coord.CreateEntity());

	std::cout << coord << std::endl;

	std::cout << "---------" << std::endl;
	coord.GetComponent<TransformTest>(e2)->position.m128_f32[0] = 666.0f;
	std::cout << coord.GetComponent<TransformTest>(e1) << std::endl;
	std::cout << coord.GetComponent<TransformTest>(e2) << std::endl;
	std::cout << "---------" << std::endl;

	{
		EntityView<TransformTest> v(coord);
		std::cout << v << std::endl;
		for (auto t : v) std::cout << " " << t << std::endl;
		std::cout << "---------" << std::endl;
	}
	{
		View<TransformTest, RigidBodyTest> v(coord);
		std::cout << v << std::endl;
		for (auto t : v) std::cout << " " << t << std::endl;
		std::cout << "---------" << std::endl;
	}
	{
		EntityView<RigidBodyTest> v(coord);
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
		std::cout << coord.GetComponents<TransformTest>() << std::endl;
		for (auto& c : coord.GetComponents<TransformTest>()) std::cout << " " << c << std::endl;
		std::cout << "---------" << std::endl;
	}

	{
		View<RigidBodyTest> v(coord);
		std::cout << v << std::endl;
		for (auto t : v) std::cout << " " << t << std::endl;
		std::cout << "---------" << std::endl;
	}

	{
		EntityView<MeshTest> v(coord);
		std::cout << v << std::endl;
		for (auto t : v) std::cout << " " << t << std::endl;
		std::cout << "---------" << std::endl;
	}
	{
		View<MeshTest> v(coord);
		std::cout << v << std::endl;
		for (auto t : v) std::cout << " " << t << std::endl;
		std::cout << "---------" << std::endl;
	}

	std::cout << "Done." << std::endl;
	return 0;
}
