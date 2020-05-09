#include "pch.h"
#include "BlockWorld.h"
#include "Engine2.h"

using namespace Engine2;
using namespace DirectX;

BlockWorld::BlockWorld() : Layer("BlockWorld")
{
	segment.total  = segment.wide * segment.high * segment.deep;
	segment.stride = segment.wide;
	segment.slice  = segment.wide * segment.high;
	instances.resize(segment.total);

	// hacking
	float centreDist = segment.cubeSize * (float)segment.wide / 2.0f;
	XMFLOAT3 center = { centreDist , centreDist , centreDist };
	segment.bounds.Center = center;
	segment.bounds.Extents = center;

	InitialiseBlocks();

	CreateVertexBuffer();

	Engine::GetActiveCamera().SetPosition(10.0f, 12.0f, -5.0f);
	Engine::GetActiveCamera().LookAt((float)segment.stride / 2.0f, (float)segment.stride / 2.0f, (float)segment.stride / 2.0f);
	//Engine::GetActiveCamera().LookAt(0.0f, 0.0f, 0.0f);
}

BlockWorld::~BlockWorld()
{
}

void BlockWorld::OnUpdate(float dt)
{
	scene.OnUpdate(dt);

	Ray ray = Engine::Get().inputController.GetRayFromMouse();

	Block* pNewHitBlock = RayHit(ray, hitDistance);
	
	if (pNewHitBlock) // if hit
	{
		if (pNewHitBlock != pHitBlock) // and it is a different or new block
		{
			if (pHitBlock) // if we had a previously hit one, revert it
			{
				pHitBlock->type = hitBlockType;
			}

			pHitBlock = pNewHitBlock;
			hitBlockType = pHitBlock->type;
			pHitBlock->type = BlockType::selected;
			instancesDirty = true;
		}
	}
	else
	{
		if (pHitBlock) // nothing hit, and if we had a previously hit one revert it
		{
			pHitBlock->type = hitBlockType;
			pHitBlock = nullptr;
			instancesDirty = true;
		}
	}
}

void BlockWorld::OnRender()
{
	scene.OnRender();

	if (instancesDirty) // only update the buffer if something changed
	{
		UpdateInstances();
		DXDevice::UpdateBuffer(instanceBuffer, instances, instanceCount);
		instanceUpdateCount++;
		instancesDirty = false;
	}

	pVS->Bind();
	pPS->Bind();
	pVB->Bind();
	pVB->Draw(instanceCount);
}

void BlockWorld::OnImgui()
{
	if (pHitBlock)
	{
		ImGui::Text("Ray hit. Distance %.3f", hitDistance);
	}
	else
	{
		ImGui::Text("Ray miss");
	}
	ImGui::Text("Instance updates %i", instanceUpdateCount);
}

BlockWorld::Block* BlockWorld::RayHit(Engine2::Ray& ray, float& distance)
{
	// If the segment is hit, then check each box in the segment.
	// To do:
	//  - Can improve speed by putting in sub-segments?
	//  - have a structure that doesn't have the empty blocks?
	float segHitDist;
	if (segment.bounds.Intersects(ray.origin, ray.direction, segHitDist))
	{
		// results
		Block* pHitBlock = nullptr;
		distance = INFINITY;

		// blocks bounding box, updated in the loop
		BoundingBox aabb;
		float halfSize = segment.cubeSize / 2.0f;
		aabb.Extents = { halfSize, halfSize, halfSize };
		// aabb.Center is update in the loop

		// block data
		Block* pBlock = blocks.data();

		// go through all the blocks
		aabb.Center.z = halfSize;
		for (UINT32 z = 0; z < segment.deep; z++)
		{
			aabb.Center.y = halfSize;
			for (UINT32 y = 0; y < segment.high; y++)
			{
				aabb.Center.x = halfSize;
				for (UINT32 x = 0; x < segment.wide; x++)
				{
					if (pBlock->type != BlockType::empty)
					{
						float newHitDist;

						if (aabb.Intersects(ray.origin, ray.direction, newHitDist))
						{
							if (newHitDist < distance)
							{
								// closer hit block
								pHitBlock = pBlock;
								distance = newHitDist;

								// break if this hit dist is the segment hit as that's closest
								if (newHitDist == segHitDist) return pHitBlock;
							}
						}
					}

					aabb.Center.x += segment.cubeSize;
					pBlock++;
				}
				aabb.Center.y += segment.cubeSize;
			}
			aabb.Center.z += segment.cubeSize;
		}

		return pHitBlock;
	}
	
	return nullptr;
}

void BlockWorld::InitialiseBlocks()
{
	blocks.resize(segment.total);

	Util::Random rng;

	Block* pBlock = blocks.data();
	float xpos, ypos, zpos;

	float distBound = pow(( segment.stride * 0.75f ), 2.0f); // temp hack

	zpos = 0.0f;
	for (UINT32 z = 0; z < segment.deep; z++)
	{
		ypos = 0.0f;
		for (UINT32 y = 0; y < segment.high; y++)
		{
			xpos = 0.0f;
			for (UINT32 x = 0; x < segment.wide; x++)
			{
				//pBlock->location = { xpos, ypos, zpos };

				float dist = xpos * xpos + ypos * ypos + zpos * zpos;
				if (dist <= distBound) pBlock->type = (rng.Next() < 0.5f ? BlockType::ground : BlockType::grass);
				else pBlock->type = BlockType::empty;

				//switch (UINT(rng.Next() * 3.0f))
				//{
				//case 0:
				//	pBlock->type = BlockType::ground;
				//	break;
				//case 1:
				//	pBlock->type = BlockType::grass;
				//	break;
				//default:
				//	pBlock->type = BlockType::empty;
				//	break;
				//}

				xpos += segment.cubeSize;
				pBlock++;
			}
			ypos += segment.cubeSize;
		}
		zpos += segment.cubeSize;
	}
}

void BlockWorld::UpdateInstances()
{
	instanceCount = 0;
	InstanceInfo* pInfo = instances.data();
	Block* pBlock = blocks.data();
	float xpos, ypos, zpos;


	zpos = 0.0f;
	for (UINT32 z = 0; z < segment.deep; z++)
	{
		ypos = 0.0f;
		for (UINT32 y = 0; y < segment.high; y++)
		{
			xpos = 0.0f;
			for (UINT32 x = 0; x < segment.wide; x++)
			{
				if (pBlock->type != BlockType::empty)
				{
					pInfo->location = { xpos, ypos, zpos };
					pInfo->type = pBlock->type;
					pInfo++;
					instanceCount++;
				}

				xpos += segment.cubeSize;
				pBlock++;
			}
			ypos += segment.cubeSize;
		}
		zpos += segment.cubeSize;
	}
}

void BlockWorld::CreateVertexBuffer()
{
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT3 normal;
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
		{"Position", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"InstanceLocation", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"BlockType", 0, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	std::vector<Vertex> verticies = {
		{ {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
		{ {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
		{ {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
		{ {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
		{ {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}  },
		{ {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}  },
		{ {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}  },
		{ {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}  },
		{ {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}  },
		{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}  },
		{ {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}  },
		{ {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}  },
		{ {0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
		{ {0.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
		{ {0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
		{ {0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
		{ {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}  },
		{ {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}  },
		{ {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}  },
		{ {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}  },
		{ {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
		{ {0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
		{ {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
		{ {1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
	};

	std::vector<unsigned int> indicies = {
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

	pVS = std::make_shared<VertexShaderDynamic>(Config::directories["ShaderSourceDir"] + "BlockWorldVS.hlsl", vsLayout);
	pPS = std::make_shared<PixelShaderDynamic>(Config::directories["ShaderSourceDir"] + "BlockWorldPS.hlsl");
	auto pVBI = std::make_shared<TriangleListIndexInstanced<Vertex, InstanceInfo>>(verticies, indicies);
	instanceBuffer = pVBI->AddInstances(instances, true);
	pVB = pVBI;
}
