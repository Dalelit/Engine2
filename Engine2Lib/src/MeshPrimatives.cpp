#include "pch.h"
#include "MeshPrimatives.h"

namespace Engine2
{
	std::vector<DirectX::XMFLOAT3> MeshPrimatives::Cube::vertexPositions = {
		{0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{1.0f, 1.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 0.0f, 1.0f},
	};

	std::vector<unsigned int> MeshPrimatives::Cube::indicies = {
		0,1,2,
		0,2,3,
		3,2,6,
		3,6,7,
		7,6,5,
		7,5,4,
		4,5,1,
		4,1,0,
		1,5,6,
		1,6,2,
		0,3,7,
		0,7,4,
	};
}