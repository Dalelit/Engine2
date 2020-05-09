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
	UpdateInstances();

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

	ray = Engine::Get().inputController.GetRayFromMouse();

	hit = segment.bounds.Intersects(ray.origin, ray.direction, hitDistance);
	if (hit)
	{
		hitLocation = ray.origin + ray.direction * hitDistance;
	}
}

void BlockWorld::OnRender()
{
	scene.OnRender();

	pVS->Bind();
	pPS->Bind();
	pVB->Bind();
	pVB->Draw(instanceCount);
}

void BlockWorld::OnImgui()
{
	ImGui::InputFloat3("Ray origin", ray.origin.m128_f32);
	ImGui::InputFloat3("Ray direction", ray.direction.m128_f32);
	if (hit)
	{
		ImGui::Text("Ray hit. Distance %.3f", hitDistance);
		ImGui::InputFloat3("Hit location", hitLocation.m128_f32);
	}
	else
	{
		ImGui::Text("Ray miss");
	}
}

bool BlockWorld::RayHit(Ray& ray, float** distance, Block** block)
{
	return false;
}

void BlockWorld::InitialiseBlocks()
{
	blocks.resize(segment.total);

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
				//if (dist <= distBound) pBlock->state = BlockState::ground;
				//else pBlock->state = BlockState::empty;
				pBlock->state = BlockState::ground;

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
				if (pBlock->state != BlockState::empty)
				{
					pInfo->location = { xpos, ypos, zpos };
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
		{"Normal",   0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"InstanceLocation", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
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
	pVBI->AddInstances(instances, true);
	pVB = pVBI;
}
