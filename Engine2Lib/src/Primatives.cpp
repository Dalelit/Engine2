#include "pch.h"
#include "Primatives.h"

using namespace DirectX;

namespace Engine2
{
	namespace Primatives
	{
		///////////////////////////////////////
		// Cube
		///////////////////////////////////////

		std::vector<PrimativesVertex> Cube::verticies = {
			{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.8f, 0.1f, 0.1f, 1.0f} },
			{ {-0.5f, 0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {0.8f, 0.1f, 0.1f, 1.0f} },
			{ {0.5f, 0.5f, -0.5f},   {0.0f, 0.0f, -1.0f}, {0.8f, 0.1f, 0.1f, 1.0f} },
			{ {0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {0.8f, 0.1f, 0.1f, 1.0f} },
			{ {0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f},  {0.1f, 0.8f, 0.1f, 1.0f} },
			{ {0.5f, 0.5f, -0.5f},   {1.0f, 0.0f, 0.0f},  {0.1f, 0.8f, 0.1f, 1.0f} },
			{ {0.5f, 0.5f, 0.5f},    {1.0f, 0.0f, 0.0f},  {0.1f, 0.8f, 0.1f, 1.0f} },
			{ {0.5f, -0.5f, 0.5f},   {1.0f, 0.0f, 0.0f},  {0.1f, 0.8f, 0.1f, 1.0f} },
			{ {0.5f, -0.5f, 0.5f},   {0.0f, 0.0f, 1.0f},  {0.1f, 0.1f, 0.8f, 1.0f} },
			{ {0.5f, 0.5f, 0.5f},    {0.0f, 0.0f, 1.0f},  {0.1f, 0.1f, 0.8f, 1.0f} },
			{ {-0.5f, 0.5f, 0.5f},   {0.0f, 0.0f, 1.0f},  {0.1f, 0.1f, 0.8f, 1.0f} },
			{ {-0.5f, -0.5f, 0.5f},  {0.0f, 0.0f, 1.0f},  {0.1f, 0.1f, 0.8f, 1.0f} },
			{ {-0.5f, -0.5f, 0.5f},  {-1.0f, 0.0f, 0.0f}, {0.8f, 0.1f, 0.8f, 1.0f} },
			{ {-0.5f, 0.5f, 0.5f},   {-1.0f, 0.0f, 0.0f}, {0.8f, 0.1f, 0.8f, 1.0f} },
			{ {-0.5f, 0.5f, -0.5f},  {-1.0f, 0.0f, 0.0f}, {0.8f, 0.1f, 0.8f, 1.0f} },
			{ {-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.8f, 0.1f, 0.8f, 1.0f} },
			{ {-0.5f, 0.5f, -0.5f},  {0.0f, 1.0f, 0.0f},  {0.8f, 0.8f, 0.1f, 1.0f} },
			{ {-0.5f, 0.5f, 0.5f},   {0.0f, 1.0f, 0.0f},  {0.8f, 0.8f, 0.1f, 1.0f} },
			{ {0.5f, 0.5f, 0.5f},    {0.0f, 1.0f, 0.0f},  {0.8f, 0.8f, 0.1f, 1.0f} },
			{ {0.5f, 0.5f, -0.5f},   {0.0f, 1.0f, 0.0f},  {0.8f, 0.8f, 0.1f, 1.0f} },
			{ {-0.5f, -0.5f, 0.5f},  {0.0f, -1.0f, 0.0f}, {0.1f, 0.8f, 0.8f, 1.0f} },
			{ {-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.1f, 0.8f, 0.8f, 1.0f} },
			{ {0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 0.0f}, {0.1f, 0.8f, 0.8f, 1.0f} },
			{ {0.5f, -0.5f, 0.5f},   {0.0f, -1.0f, 0.0f}, {0.1f, 0.8f, 0.8f, 1.0f} },
		};

		std::vector<unsigned int> Cube::indicies = {
			0, 1, 2,
			0, 2, 3,
			4, 5, 6,
			4, 6, 7,
			8, 9, 10,
			8, 10, 11,
			12, 13, 14,
			12, 14, 15,
			16, 17, 18,
			16, 18, 19,
			20, 21, 22,
			20, 22, 23,
		};

		///////////////////////////////////////
		// CubeWireframe
		///////////////////////////////////////

		std::vector<XMFLOAT3> CubeWireframe::verticies = {
			{-0.5f, -0.5f, -0.5f},
			{0.5f, -0.5f, -0.5f},
			{0.5f, 0.5f, -0.5f},
			{-0.5f, 0.5f, -0.5f},
			{-0.5f, -0.5f, 0.5f},
			{0.5f, -0.5f, 0.5f},
			{0.5f, 0.5f, 0.5f},
			{-0.5f, 0.5f, 0.5f},
		};

		std::vector<unsigned int> CubeWireframe::indicies = {
			0,1,1,2,2,3,3,0,
			4,5,5,6,6,7,7,4,
			0,4,1,5,2,6,3,7,
		};

		///////////////////////////////////////
		// Icosphere
		///////////////////////////////////////
		// http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html

		PrimativesData IcoSphere::CreateIcoSphere(unsigned int iterations)
		{
			PrimativesData data;
			data.verticies = std::make_shared<std::vector<PrimativesVertex>>();
			data.indicies  = std::make_shared<std::vector<unsigned int>>();

			std::vector<XMVECTOR> positions;
			std::vector<unsigned int>& indicies = *data.indicies;

			// note - swapping the order of the indicies from the code sample
			auto addTriangleIndicies = [&indicies](unsigned int i0, unsigned int i1, unsigned int i2) mutable {
				indicies.push_back(i0);
				indicies.push_back(i2);
				indicies.push_back(i1);
			};

			// Create a new vertex between 2 others. Adjust to unit sphere.
			// Return the index of the created vertex
			auto addMidpointVertex = [&positions](unsigned int i0, unsigned int i1) mutable -> unsigned int {
				XMVECTOR v0 = positions[i0];
				XMVECTOR v1 = positions[i1];
				XMVECTOR vmid = XMVector3Normalize((v0 + v1) / 2.0f); // new position

				auto current = positions.begin();
				auto end = positions.end();
				unsigned int indx = 0;

				while (current != end && !XMVector3Equal(*current, vmid))
				{
					current++;
					indx++;
				}

				if (current == end) positions.push_back(vmid); // not found so add it

				return indx;
			};

			// create 12 vertices of a icosahedron
			float t = (1.0f + sqrtf(5.0f)) / 2.0f;

			positions.push_back(XMVector3Normalize({ -1.0f, t, 0.0f, 1.0f }));
			positions.push_back(XMVector3Normalize({ 1.0f, t, 0.0f, 1.0f }));
			positions.push_back(XMVector3Normalize({ -1.0f, -t, 0.0f, 1.0f }));
			positions.push_back(XMVector3Normalize({ 1.0f, -t, 0.0f, 1.0f }));

			positions.push_back(XMVector3Normalize({ 0.0f, -1.0f, t, 1.0f }));
			positions.push_back(XMVector3Normalize({ 0.0f, 1.0f, t, 1.0f }));
			positions.push_back(XMVector3Normalize({ 0.0f, -1.0f, -t, 1.0f }));
			positions.push_back(XMVector3Normalize({ 0.0f, 1.0f, -t, 1.0f }));

			positions.push_back(XMVector3Normalize({ t, 0.0f, -1.0f, 1.0f }));
			positions.push_back(XMVector3Normalize({ t, 0.0f, 1.0f, 1.0f }));
			positions.push_back(XMVector3Normalize({ -t, 0.0f, -1.0f, 1.0f }));
			positions.push_back(XMVector3Normalize({ -t, 0.0f, 1.0f, 1.0f }));

			// create 20 triangles of the icosahedron

			// 5 faces around point 0
			addTriangleIndicies(0, 11, 5);
			addTriangleIndicies(0, 5, 1);
			addTriangleIndicies(0, 1, 7);
			addTriangleIndicies(0, 7, 10);
			addTriangleIndicies(0, 10, 11);

			// 5 adjacent faces
			addTriangleIndicies(1, 5, 9);
			addTriangleIndicies(5, 11, 4);
			addTriangleIndicies(11, 10, 2);
			addTriangleIndicies(10, 7, 6);
			addTriangleIndicies(7, 1, 8);

			// 5 faces around point 3
			addTriangleIndicies(3, 9, 4);
			addTriangleIndicies(3, 4, 2);
			addTriangleIndicies(3, 2, 6);
			addTriangleIndicies(3, 6, 8);
			addTriangleIndicies(3, 8, 9);

			// 5 adjacent faces
			addTriangleIndicies(4, 9, 5);
			addTriangleIndicies(2, 4, 11);
			addTriangleIndicies(6, 2, 10);
			addTriangleIndicies(8, 6, 7);
			addTriangleIndicies(9, 8, 1);

			// created the core object.

			// now iterate over the triangles dividing them.
			for (unsigned int iter = 0; iter < iterations; iter++)
			{
				auto current = indicies.begin();
				auto end = indicies.end();

				std::vector<unsigned int> newIndicies;

				while (current != end)
				{
					unsigned int i0 = *current;
					unsigned int i1 = *(current + 1);
					unsigned int i2 = *(current + 2);

					unsigned int i01 = addMidpointVertex(i0, i1);
					unsigned int i12 = addMidpointVertex(i1, i2);
					unsigned int i20 = addMidpointVertex(i2, i0);

					// change the current triangle to be a sub triangle
					// current = i0; // unchanged
					current++;
					*current = i01;
					current++;
					*current = i20;
					current++;

					// add the 3 new triangles
					newIndicies.push_back(i01); newIndicies.push_back(i1); newIndicies.push_back(i12);
					newIndicies.push_back(i01); newIndicies.push_back(i12); newIndicies.push_back(i20);
					newIndicies.push_back(i20); newIndicies.push_back(i12); newIndicies.push_back(i2);
				}

				// add the new ones to the indicies
				for (auto indx : newIndicies) indicies.push_back(indx);
			}

			// convert positions to primative verticies
			for (auto v : positions)
			{
				PrimativesVertex newVertex;
				XMStoreFloat3(&newVertex.position, v * 0.5f);
				XMStoreFloat3(&newVertex.normal, v); // calc normals

				XMVECTOR color = (v + g_XMOne) / 2.0f;
				XMStoreFloat4(&newVertex.color, color);
				
				data.verticies->push_back(newVertex);
			}

			return data;

			// to do: untested
			// Calc texture uv
			//for (auto n : normals)
			//{
			//	float tu = asinf(n.m128_f32[0]) / XM_PI + 0.5f;
			//	float tv = asinf(n.m128_f32[1]) / XM_PI + 0.5f;
			//}
		}
	}
}