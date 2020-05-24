#include "pch.h"
#include "BlockWorld.h"
#include "Engine2.h"
#include "VertexBuffer.h"
#include "UtilMath.h"

using namespace Engine2;
using namespace DirectX;

namespace BlockWorld
{

	BlockWorld::BlockWorld() : Layer("BlockWorld")
	{
		//segment.total = segment.wide * segment.high * segment.deep;
		//segment.stride = segment.wide;
		//segment.slice = segment.wide * segment.high;
		//instances.resize(segment.total);

		// hacking
		//float centreDist = segment.cubeSize * (float)segment.wide / 2.0f;
		//XMFLOAT3 center = { centreDist , centreDist , centreDist };
		//segment.bounds.Center = center;
		//segment.bounds.Extents = center;

		//InitialiseBlocks();

		//CreateVertexBuffer();
		//CreateVertexBufferHighlight();

		Engine::GetActiveCamera().SetPosition(10.0f, 6.0f, -5.0f);
		//Engine::GetActiveCamera().LookAt((float)segment.stride / 2.0f, (float)segment.stride / 2.0f, (float)segment.stride / 2.0f);
		Engine::GetActiveCamera().LookAt(0.0f, 0.0f, 0.0f);

		//Engine::GetInputController().State.MouseLook = true;
		Engine::GetInputController().MovementConfiguration.moveSpeed = 10.0f;
	}

	BlockWorld::~BlockWorld()
	{
	}

	void BlockWorld::OnUpdate(float dt)
	{
		scene.OnUpdate(dt);

		Ray ray;
		//if (Engine::GetInputController().State.MouseLook)
		//	ray = Engine::GetInputController().GetRayForward();
		//else
		ray = Engine::GetInputController().GetRayFromMouse();

		blockHit = chunkMgr.Intersects(ray.origin, ray.direction, hitInfo);

		if (blockHit)
		{
			hitLocation = ray.origin + ray.direction * hitInfo.distance;
		}

		//Block* pNewHitBlock = RayHit(ray, hitDistance);

		//if (pNewHitBlock) // if hit
		//{
		//	hitLocation = ray.origin + ray.direction * hitDistance;

		//	hitBlockIndx.x = (INT)hitBlockCentre.x;
		//	hitBlockIndx.y = (INT)hitBlockCentre.y;
		//	hitBlockIndx.z = (INT)hitBlockCentre.z;

		//	if (pNewHitBlock != pHitBlock) // and it is a different or new block
		//	{
		//		pHitBlock = pNewHitBlock;
		//		instancesDirty = true;
		//	}

		//	SetNextBlockIndx();
		//}
		//else
		//{
		//	if (pHitBlock) // nothing hit, and if we had a previously hit one revert it
		//	{
		//		pHitBlock = nullptr;
		//		instancesDirty = true;
		//	}
		//}
	}

	void BlockWorld::OnRender()
	{
		//offscreen.Clear();
		//offscreen.SetAsTarget();

		scene.OnRender();

		//if (instancesDirty) // only update the buffer if something changed
		//{
		//	UpdateInstances();
		//	DXDevice::UpdateBuffer(instanceBuffer, instances, instanceCount);
		//	instanceUpdateCount++;
		//	instancesDirty = false;
		//}

		//pVS->Bind();
		//pPS->Bind();
		//pVB->Bind();
		//pVB->Draw(instanceCount);

		chunkMgr.pPS->Bind();
		chunkMgr.pVS->Bind();

		Chunk* pChunk = chunkMgr.ChunksBegin();
		Chunk* pChunkEnd = chunkMgr.ChunksEnd();

		while (pChunk != pChunkEnd)
		{
			if (pChunk->pVB) pChunk->pVB->BindAndDraw();

			pChunk++;
		}

		//if (pHitBlock)
		//{
		//	highlighter.pVSCB->data = XMLoadFloat3(&hitBlockCentre);
		//	highlighter.pVSCB->Bind();
		//	highlighter.pVS->Bind();
		//	highlighter.pPSCB->data = highlighter.color;
		//	highlighter.pPSCB->Bind();
		//	highlighter.pPS->Bind();
		//	highlighter.pVB->BindAndDraw();

		//	DXDevice::GetContext().OMSetDepthStencilState(highlighter.pBackDrawDSS.Get(), 0);
		//	highlighter.pPSCB->data = highlighter.hiddenColor;
		//	highlighter.pPSCB->Bind();
		//	highlighter.pVB->Draw();

		//	DXDevice::Get().SetDefaultDepthStencilState(); // revert back to default

		//	highlighter.pVSCB->data = XMLoadFloat3(&nextBlockCentre);
		//	highlighter.pVSCB->Bind();
		//	highlighter.pPSCB->data = highlighter.nextColor;
		//	highlighter.pPSCB->Bind();
		//	highlighter.pVB->Draw();
		//}

		//offscreen.DrawToBackBuffer();
		//offscreen.ShowSubDisplayRenderTarget();
		//offscreen.ShowSubDisplayDepthBuffer();
	}

	void BlockWorld::OnInputEvent(InputEvent& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<MouseButtonReleasedEvent>(E2_BIND_EVENT_FUNC(BlockWorld::MouseButtonReleased));
		dispatcher.Dispatch<KeyPressedEvent>(E2_BIND_EVENT_FUNC(BlockWorld::KeyPressed));
	}

	void BlockWorld::OnApplicationEvent(ApplicationEvent& event)
	{
		if (event.GetType() == EventType::WindowResize) offscreen.Reconfigure();
	}

	void BlockWorld::OnImgui()
	{
		ImGui::Text("Press 'M' to toggle mouse look and cursor clipping on/off.");

		if (ImGui::Checkbox("Wireframe", &wireframeOn)) {
			if (wireframeOn) DXDevice::Get().SetWireframeRenderState(false);
			else DXDevice::Get().SetDefaultRenderState();
		}

		if (blockHit)
		{
			ImGui::Text("Ray hit");
			ImGui::Text("Hit distance %.3f", hitInfo.distance);
			ImGui::Text("Block centre %.3f,%.3f,%.3f", hitInfo.centre.x, hitInfo.centre.y, hitInfo.centre.z);
			ImGui::Text("Hit location %.3f,%.3f,%.3f", hitLocation.m128_f32[0], hitLocation.m128_f32[1], hitLocation.m128_f32[2]);
			ImGui::Text("Chunk hit %i,%i,%i", hitInfo.pChunk->index.x, hitInfo.pChunk->index.y, hitInfo.pChunk->index.z);
		}
		else
		{
			ImGui::Text("Ray miss");
		}

		chunkMgr.OnImgui();

		//offscreen.OnImgui();
		//if (pHitBlock)
		//{
		//	ImGui::Text("Ray hit. Distance %.3f", hitDistance);
		//	ImGui::InputInt3("Hit block index", &hitBlockIndx.x);
		//	ImGui::InputInt3("New block", &nextBlockIndx.x);
		//	ImGui::InputFloat3("Hit location", hitLocation.m128_f32);
		//	ImGui::InputFloat3("Hit block centre", &hitBlockCentre.x);
		//}
		//else
		//{
		//	ImGui::Text("Ray miss");
		//}
		//ImGui::Text("Instance updates %i", instanceUpdateCount);
	}

	void BlockWorld::OnGizmos()
	{
		if (blockHit)
		{
			XMFLOAT3 blockOrigin = hitInfo.centre;
			blockOrigin.x -= chunkMgr.BlockSpecs.halfSize;
			blockOrigin.y -= chunkMgr.BlockSpecs.halfSize;
			blockOrigin.z -= chunkMgr.BlockSpecs.halfSize;
			gizmos.DrawAxis(Math::TranslationMatrix(blockOrigin));
			gizmos.DrawSphere(XMMatrixScaling(0.5f, 0.5f, 0.5f) * Math::TranslationMatrix(hitLocation));
		}
	}

	//BlockWorld::Block* BlockWorld::RayHit(Engine2::Ray& ray, float& distance)
	//{
	//	// If the segment is hit, then check each box in the segment.
	//	// To do:
	//	//  - Can improve speed by putting in sub-segments?
	//	//  - have a structure that doesn't have the empty blocks?
	//	float segHitDist;
	//	if (segment.bounds.Intersects(ray.origin, ray.direction, segHitDist))
	//	{
	//		// results
	//		Block* pHitBlock = nullptr;
	//		distance = INFINITY;

	//		// blocks bounding box, updated in the loop
	//		BoundingBox aabb;
	//		float halfSize = segment.cubeSize / 2.0f;
	//		aabb.Extents = { halfSize, halfSize, halfSize };
	//		// aabb.Center is update in the loop

	//		// block data
	//		Block* pBlock = blocks.data();

	//		// go through all the blocks
	//		aabb.Center.z = halfSize;
	//		for (INT32 z = 0; z < segment.deep; z++)
	//		{
	//			aabb.Center.y = halfSize;
	//			for (INT32 y = 0; y < segment.high; y++)
	//			{
	//				aabb.Center.x = halfSize;
	//				for (INT32 x = 0; x < segment.wide; x++)
	//				{
	//					if (pBlock->type != BlockType::empty)
	//					{
	//						float newHitDist;

	//						if (aabb.Intersects(ray.origin, ray.direction, newHitDist))
	//						{
	//							if (newHitDist < distance)
	//							{
	//								// closer hit block
	//								pHitBlock = pBlock;
	//								distance = newHitDist;
	//								hitBlockCentre = aabb.Center;

	//								// break if this hit dist is the segment hit as that's closest
	//								if (newHitDist == segHitDist) return pHitBlock;
	//							}
	//						}
	//					}

	//					aabb.Center.x += segment.cubeSize;
	//					pBlock++;
	//				}
	//				aabb.Center.y += segment.cubeSize;
	//			}
	//			aabb.Center.z += segment.cubeSize;
	//		}

	//		return pHitBlock;
	//	}

	//	return nullptr;
	//}

	//void BlockWorld::SetNextBlockIndx()
	//{
	//	XMFLOAT3 diff = { hitLocation.m128_f32[0] - hitBlockCentre.x, hitLocation.m128_f32[1] - hitBlockCentre.y, hitLocation.m128_f32[2] - hitBlockCentre.z };
	//	nextBlockIndx = hitBlockIndx;
	//	nextBlockCentre = hitBlockCentre;

	//	float xabs = fabsf(diff.x);
	//	float yabs = fabsf(diff.y);
	//	float zabs = fabsf(diff.z);

	//	if (xabs > yabs)
	//	{
	//		if (xabs > zabs)
	//		{
	//			// x biggest diff
	//			if (diff.x > 0.0f) { nextBlockIndx.x++; nextBlockCentre.x += segment.cubeSize; }
	//			else { nextBlockIndx.x--; nextBlockCentre.x -= segment.cubeSize; }
	//		}
	//		else
	//		{
	//			// z biggest diff
	//			if (diff.z > 0.0f) { nextBlockIndx.z++; nextBlockCentre.z += segment.cubeSize; }
	//			else { nextBlockIndx.z--; nextBlockCentre.z -= segment.cubeSize; }
	//		}
	//	}
	//	else // y > x
	//	{
	//		if (yabs > zabs)
	//		{
	//			// y biggest diff
	//			if (diff.y > 0.0f) { nextBlockIndx.y++; nextBlockCentre.y += segment.cubeSize; }
	//			else { nextBlockIndx.y--; nextBlockCentre.y -= segment.cubeSize; }
	//		}
	//		else
	//		{
	//			// z biggest diff
	//			if (diff.z > 0.0f) { nextBlockIndx.z++; nextBlockCentre.z += segment.cubeSize; }
	//			else { nextBlockIndx.z--; nextBlockCentre.z -= segment.cubeSize; }
	//		}
	//	}

	//}

	//void BlockWorld::AddNextBlock()
	//{
	//	Block* pBlock = GetBlock(nextBlockIndx);

	//	if (pBlock)
	//	{
	//		E2_ASSERT(pBlock->type == BlockType::empty, "Expected the next block to be empty");

	//		pBlock->type = BlockType::grass;
	//		instancesDirty = true;
	//		pHitBlock = nullptr;
	//	}
	//}

	//void BlockWorld::RemoveBlock()
	//{
	//	pHitBlock->type = BlockType::empty;
	//}

	//BlockWorld::Block* BlockWorld::GetBlock(DirectX::XMINT3& index)
	//{
	//	if (index.x < 0 || index.y < 0 || index.z < 0) return nullptr;
	//	if (index.x >= segment.wide || index.y >= segment.high || index.z >= segment.deep) return nullptr;

	//	return &blocks[index.z * segment.slice + index.y * segment.stride + index.x];
	//}

	void BlockWorld::MouseButtonReleased(Engine2::MouseButtonReleasedEvent& event)
	{
		//if (pHitBlock)
		//{
		//	if (event.Right())
		//	{
		//		AddNextBlock();
		//	}
		//	else if (event.Left())
		//	{
		//		RemoveBlock();
		//	}
		//}
	}

	void BlockWorld::KeyPressed(Engine2::KeyPressedEvent& event)
	{
		if (event.GetKey() == 'M')
		{
			auto& ic = Engine::GetInputController();
			if (ic.State.MouseLook)
			{
				ic.State.MouseLook = false;
				ic.SetCursorClipping(false);
			}
			else
			{
				ic.State.MouseLook = true;
				ic.SetCursorClipping(true);
			}
		}
	}

	//void BlockWorld::InitialiseBlocks()
	//{
	//	blocks.resize(segment.total);

	//	Util::Random rng;

	//	Block* pBlock = blocks.data();
	//	float xpos, ypos, zpos;

	//	float distBound = pow((segment.stride * 0.75f), 2.0f); // temp hack

	//	zpos = 0.0f;
	//	for (INT32 z = 0; z < segment.deep; z++)
	//	{
	//		ypos = 0.0f;
	//		for (INT32 y = 0; y < segment.high; y++)
	//		{
	//			xpos = 0.0f;
	//			for (INT32 x = 0; x < segment.wide; x++)
	//			{
	//				//pBlock->location = { xpos, ypos, zpos };

	//				float dist = xpos * xpos + ypos * ypos + zpos * zpos;
	//				if (dist <= distBound) pBlock->type = (rng.Next() < 0.5f ? BlockType::ground : BlockType::grass);
	//				else pBlock->type = BlockType::empty;

	//				//switch (UINT(rng.Next() * 3.0f))
	//				//{
	//				//case 0:
	//				//	pBlock->type = BlockType::ground;
	//				//	break;
	//				//case 1:
	//				//	pBlock->type = BlockType::grass;
	//				//	break;
	//				//default:
	//				//	pBlock->type = BlockType::empty;
	//				//	break;
	//				//}

	//				xpos += segment.cubeSize;
	//				pBlock++;
	//			}
	//			ypos += segment.cubeSize;
	//		}
	//		zpos += segment.cubeSize;
	//	}
	//}

	//void BlockWorld::UpdateInstances()
	//{
	//	instanceCount = 0;
	//	InstanceInfo* pInfo = instances.data();
	//	Block* pBlock = blocks.data();
	//	float xpos, ypos, zpos;


	//	zpos = 0.0f;
	//	for (INT32 z = 0; z < segment.deep; z++)
	//	{
	//		ypos = 0.0f;
	//		for (INT32 y = 0; y < segment.high; y++)
	//		{
	//			xpos = 0.0f;
	//			for (INT32 x = 0; x < segment.wide; x++)
	//			{
	//				if (pBlock->type != BlockType::empty)
	//				{
	//					pInfo->location = { xpos, ypos, zpos };
	//					pInfo->type = pBlock->type;
	//					pInfo++;
	//					instanceCount++;
	//				}

	//				xpos += segment.cubeSize;
	//				pBlock++;
	//			}
	//			ypos += segment.cubeSize;
	//		}
	//		zpos += segment.cubeSize;
	//	}
	//}

	//void BlockWorld::CreateVertexBuffer()
	//{
	//	struct Vertex {
	//		XMFLOAT3 position;
	//		XMFLOAT3 normal;
	//	};

	//	std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
	//		{"Position", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
	//		{"Normal", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
	//		{"InstanceLocation", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
	//		{"BlockType", 0, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
	//	};

	//	std::vector<Vertex> verticies = {
	//		{ {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
	//		{ {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
	//		{ {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
	//		{ {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
	//		{ {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}  },
	//		{ {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}  },
	//		{ {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}  },
	//		{ {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}  },
	//		{ {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}  },
	//		{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}  },
	//		{ {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}  },
	//		{ {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}  },
	//		{ {0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
	//		{ {0.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
	//		{ {0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
	//		{ {0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
	//		{ {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}  },
	//		{ {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}  },
	//		{ {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}  },
	//		{ {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}  },
	//		{ {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
	//		{ {0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
	//		{ {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
	//		{ {1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
	//	};

	//	std::vector<UINT> indicies = {
	//		0, 1, 2, 0, 2, 3,
	//		4, 5, 6, 4, 6, 7,
	//		8, 9, 10, 8, 10, 11,
	//		12, 13, 14, 12, 14, 15,
	//		16, 17, 18, 16, 18, 19,
	//		20, 21, 22, 20, 22, 23,
	//	};

	//	struct face
	//	{
	//		Vertex* firstVertex;
	//		UINT* firstIndex;
	//	};

	//	std::vector<face> faces(6);


	//	pVS = std::make_shared<VertexShaderDynamic>(Config::directories["ShaderSourceDir"] + "BlockWorldVS.hlsl", vsLayout);
	//	pPS = std::make_shared<PixelShaderDynamic>(Config::directories["ShaderSourceDir"] + "BlockWorldPS.hlsl");
	//	auto pVBI = std::make_shared<TriangleListIndexInstanced<Vertex, InstanceInfo>>(verticies, indicies);
	//	instanceBuffer = pVBI->AddInstances(instances, true);
	//	pVB = pVBI;

	//}

	//void BlockWorld::CreateVertexBufferHighlight()
	//{
	//	std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
	//		{"Position", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	};

	//	float extents = 0.501f;
	//	std::vector<XMFLOAT3> verticies = {
	//		{-extents, -extents, -extents},
	//		{extents, -extents, -extents},
	//		{extents, extents, -extents},
	//		{-extents, extents, -extents},
	//		{-extents, -extents, extents},
	//		{extents, -extents, extents},
	//		{extents, extents, extents},
	//		{-extents, extents, extents},
	//	};

	//	std::vector<unsigned int> indicies = {
	//		0,1,1,2,2,3,3,0,
	//		4,5,5,6,6,7,7,4,
	//		0,4,1,5,2,6,3,7,
	//	};

	//	highlighter.pVS = std::make_shared<VertexShaderDynamic>(Config::directories["ShaderSourceDir"] + "BlockWorldHighlightVS.hlsl", vsLayout);
	//	highlighter.pPS = std::make_shared<PixelShaderDynamic>(Config::directories["ShaderSourceDir"] + "BlockWorldHighlightPS.hlsl");
	//	highlighter.pVB = std::make_shared<WireframeIndexList<XMFLOAT3>>(verticies, indicies);
	//	highlighter.pVSCB = std::make_shared<VSConstantBuffer<XMVECTOR>>(1);
	//	highlighter.pPSCB = std::make_shared<PSConstantBuffer<XMVECTOR>>(0);

	//	D3D11_DEPTH_STENCIL_DESC desc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	//	desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER;
	//	DXDevice::GetDevice().CreateDepthStencilState(&desc, &highlighter.pBackDrawDSS);
	//}

}