#include "pch.h"
#include "BlockWorldChunk.h"
#include "VertexBuffer.h"

using namespace DirectX;
using namespace Engine2;

namespace BlockWorld
{
	ChunkManager::ChunkManager()
	{
		Initialise();
		InitialiseChunks();

		for (auto& chunk : Chunks()) InitialiseBlockFaces(&chunk);
		for (auto& chunk : Chunks()) SetVertexBuffer(&chunk);

		VertexData vd;

		pVS = std::make_shared<VertexShaderFile>(Config::directories["ShaderSourceDir"] + "BlockWorldVS.hlsl", vd.vsLayout);
		pPS = std::make_shared<PixelShaderFile>(Config::directories["ShaderSourceDir"] + "BlockWorldPS.hlsl");
	}

	ChunkManager::~ChunkManager()
	{
	}

	bool ChunkManager::Intersects(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDirection, HitInfo& hitInfo)
	{
		hitInfo.pBlock = nullptr;
		hitInfo.pChunk = nullptr;
		hitInfo.distance = INFINITY;
		float hitDist;

		XMVECTOR rayDirSmallInc = rayDirection * 0.0001f;
		BoundingBox blockBoundingBox;
		blockBoundingBox.Extents = { BlockSpecs.halfSize, BlockSpecs.halfSize, BlockSpecs.halfSize };

		bool intersected = false;

		if (worldBounds.Contains(rayOrigin))
		{
			intersected = true;
			hitDist = 0.0f;
		}
		else if (worldBounds.Intersects(rayOrigin, rayDirection, hitDist))
		{
			intersected = true;
		}

		while (intersected)
		{
			XMVECTOR hitPosition = rayOrigin + rayDirection * hitDist;
			XMVECTOR nextOrigin = hitPosition + rayDirSmallInc;

			if (!worldBounds.Contains(nextOrigin)) return false;

			XMINT3 chunkIndx = GetChunkIndex(nextOrigin);

			Chunk* pChunk = GetChunk(chunkIndx);
			XMINT3 blockIndx = GetBlockIndex(pChunk, nextOrigin);

			Block* pBlock = GetBlock(pChunk, blockIndx);
			blockBoundingBox.Center = GetBlockCentre(pChunk, blockIndx);

			if (pBlock->type != BlockType::BlockTypeEmpty)
			{
				hitInfo.blockIndex = blockIndx;
				hitInfo.pBlock = pBlock;
				hitInfo.pChunk = pChunk;
				hitInfo.centre = blockBoundingBox.Center;
				hitInfo.distance = hitDist;
				return true;
			}

			// calculate next hitDist
			{
				XMVECTOR floorDist = XMVectorFloor(nextOrigin);
				XMVECTOR ceilingDist = XMVectorCeiling(nextOrigin);

				floorDist -= rayOrigin;
				ceilingDist -= rayOrigin;

				// depending on direction of the ray, get either the floor or ceiling dist
				XMVECTOR t;
				if (rayDirection.m128_f32[0] > 0.0f) t.m128_f32[0] = ceilingDist.m128_f32[0];
				else t.m128_f32[0] = floorDist.m128_f32[0];
				if (rayDirection.m128_f32[1] > 0.0f) t.m128_f32[1] = ceilingDist.m128_f32[1];
				else t.m128_f32[1] = floorDist.m128_f32[1];
				if (rayDirection.m128_f32[2] > 0.0f) t.m128_f32[2] = ceilingDist.m128_f32[2];
				else t.m128_f32[2] = floorDist.m128_f32[2];

				// set to be a ratio of the direction
				t /= rayDirection;
				t = XMVectorAbs(t);

				// find the smallest t for the hitDist
				hitDist = (t.m128_f32[0] < t.m128_f32[1] ? t.m128_f32[0] : t.m128_f32[1]);
				if (t.m128_f32[2] < hitDist) hitDist = t.m128_f32[2];
			}
		}

		return false;
	}

	void ChunkManager::RemoveBlock(HitInfo& hitInfo)
	{
		E2_ASSERT(hitInfo.pBlock != nullptr && hitInfo.pChunk != nullptr, "Remove block expected pointer to block and chunk");
		E2_ASSERT(hitInfo.pBlock->type != BlockType::BlockTypeEmpty, "Remove block expected to point to an non-empty block");

		hitInfo.pBlock->type = BlockType::BlockTypeEmpty;
		//hitInfo.pBlock->faceMask = FaceMask::FaceMaskNone;

		InitialiseBlockFaces(hitInfo.pChunk);
		SetVertexBuffer(hitInfo.pChunk);

		// check if at edges.
		std::vector<Chunk*> neighbours;
		GetNeighbours(hitInfo, neighbours);
		if (neighbours.size() > 0)
		{
			for (auto pN : neighbours)
			{
				InitialiseBlockFaces(pN);
				SetVertexBuffer(pN);
			}
		}
	}

	void ChunkManager::AddBlock(HitInfo& hitInfo, DirectX::XMVECTOR& hitLocation)
	{
		E2_ASSERT(hitInfo.pBlock != nullptr && hitInfo.pChunk != nullptr, "Add block expected pointer to block and chunk");
		E2_ASSERT(hitInfo.pBlock->type != BlockType::BlockTypeEmpty, "Remove block expected to point to an non-empty block");

		Block* pNextBlock = nullptr;

		float xdist = hitLocation.m128_f32[0] - hitInfo.centre.x;
		float ydist = hitLocation.m128_f32[1] - hitInfo.centre.y;
		float zdist = hitLocation.m128_f32[2] - hitInfo.centre.z;

		float xabs = fabsf(xdist);
		float yabs = fabsf(ydist);
		float zabs = fabsf(zdist);

		float maxdist = max(max(xabs, yabs), zabs);

		if (xabs == maxdist)
		{
			if (xdist > 0) pNextBlock = GetBlockNextX(hitInfo.pChunk, hitInfo.blockIndex);
			else pNextBlock = GetBlockPrevX(hitInfo.pChunk, hitInfo.blockIndex);
		}
		else if (yabs == maxdist)
		{
			if (ydist > 0) pNextBlock = GetBlockNextY(hitInfo.pChunk, hitInfo.blockIndex);
			else pNextBlock = GetBlockPrevY(hitInfo.pChunk, hitInfo.blockIndex);
		}
		else // zabs == maxdist
		{
			if (zdist > 0) pNextBlock = GetBlockNextZ(hitInfo.pChunk, hitInfo.blockIndex);
			else pNextBlock = GetBlockPrevZ(hitInfo.pChunk, hitInfo.blockIndex);
		}

		if (pNextBlock)
		{
			pNextBlock->type = BlockType::BlockTypeDirt;
			InitialiseBlockFaces(hitInfo.pChunk);
			SetVertexBuffer(hitInfo.pChunk);

			// check if at edges.
			std::vector<Chunk*> neighbours;
			GetNeighbours(hitInfo, neighbours);
			if (neighbours.size() > 0)
			{
				for (auto pN : neighbours)
				{
					InitialiseBlockFaces(pN);
					SetVertexBuffer(pN);
				}
			}
		}
	}

	void ChunkManager::GetNeighbours(HitInfo& hitInfo, std::vector<Chunk*>& neighbours)
	{
		XMINT3& ci = hitInfo.pChunk->index;
		XMINT3& bi = hitInfo.blockIndex;

		if (bi.x == 0 && ci.x != 0) neighbours.push_back(GetChunk(ci.x - 1, ci.y, ci.z));
		else if (bi.x == ChunkSpecs.wide - 1 && ci.x != WorldSpecs.wide - 1) neighbours.push_back(GetChunk(ci.x + 1, ci.y, ci.z));

		if (bi.y == 0 && ci.y != 0) neighbours.push_back(GetChunk(ci.x, ci.y - 1, ci.z));
		else if (bi.y == ChunkSpecs.high - 1 && ci.y != WorldSpecs.high - 1) neighbours.push_back(GetChunk(ci.x, ci.y + 1, ci.z));

		if (bi.z == 0 && ci.z != 0) neighbours.push_back(GetChunk(ci.x, ci.y, ci.z - 1));
		else if (bi.z == ChunkSpecs.deep - 1 && ci.z != WorldSpecs.deep - 1) neighbours.push_back(GetChunk(ci.x, ci.y, ci.z + 1));
	}

	Block* ChunkManager::GetBlockPrevX(Chunk* pChunk, DirectX::XMINT3& blockIndx)
	{
		if (blockIndx.x > 0) return GetBlock(pChunk, blockIndx.x - 1, blockIndx.y, blockIndx.z);

		if (pChunk->index.x > 0)
		{
			Chunk* pPrevChunk = GetChunk(pChunk->index.x - 1, pChunk->index.y, pChunk->index.z);
			return GetBlock(pPrevChunk, ChunkSpecs.wide - 1, blockIndx.y, blockIndx.z);
		}

		return nullptr;
	}

	Block* ChunkManager::GetBlockNextX(Chunk* pChunk, DirectX::XMINT3& blockIndx)
	{
		if (blockIndx.x < ChunkSpecs.wide - 1) return GetBlock(pChunk, blockIndx.x + 1, blockIndx.y, blockIndx.z);

		if (pChunk->index.x < WorldSpecs.wide - 1)
		{
			Chunk* pNextChunk = GetChunk(pChunk->index.x + 1, pChunk->index.y, pChunk->index.z);
			return GetBlock(pNextChunk, 0, blockIndx.y, blockIndx.z);
		}

		return nullptr;
	}

	Block* ChunkManager::GetBlockPrevY(Chunk* pChunk, DirectX::XMINT3& blockIndx)
	{
		if (blockIndx.y > 0) return GetBlock(pChunk, blockIndx.x, blockIndx.y - 1, blockIndx.z);

		if (pChunk->index.y > 0)
		{
			Chunk* pPrevChunk = GetChunk(pChunk->index.x, pChunk->index.y - 1, pChunk->index.z);
			return GetBlock(pPrevChunk, blockIndx.x, ChunkSpecs.high - 1, blockIndx.z);
		}

		return nullptr;
	}

	Block* ChunkManager::GetBlockNextY(Chunk* pChunk, DirectX::XMINT3& blockIndx)
	{
		if (blockIndx.y < ChunkSpecs.high - 1) return GetBlock(pChunk, blockIndx.x, blockIndx.y + 1, blockIndx.z);

		if (pChunk->index.y < WorldSpecs.high - 1)
		{
			Chunk* pNextChunk = GetChunk(pChunk->index.x, pChunk->index.y + 1, pChunk->index.z);
			return GetBlock(pNextChunk, blockIndx.x, 0, blockIndx.z);
		}

		return nullptr;
	}

	Block* ChunkManager::GetBlockPrevZ(Chunk* pChunk, DirectX::XMINT3& blockIndx)
	{
		if (blockIndx.z > 0) return GetBlock(pChunk, blockIndx.x, blockIndx.y, blockIndx.z - 1);

		if (pChunk->index.z > 0)
		{
			Chunk* pPrevChunk = GetChunk(pChunk->index.x, pChunk->index.y, pChunk->index.z - 1);
			return GetBlock(pPrevChunk, blockIndx.x, blockIndx.y, ChunkSpecs.deep - 1);
		}

		return nullptr;
	}

	Block* ChunkManager::GetBlockNextZ(Chunk* pChunk, DirectX::XMINT3& blockIndx)
	{
		if (blockIndx.z < ChunkSpecs.deep - 1) return GetBlock(pChunk, blockIndx.x, blockIndx.y, blockIndx.z + 1);

		if (pChunk->index.z < WorldSpecs.deep - 1)
		{
			Chunk* pNextChunk = GetChunk(pChunk->index.x, pChunk->index.y, pChunk->index.z + 1);
			return GetBlock(pNextChunk, blockIndx.x, blockIndx.y, 0);
		}

		return nullptr;
	}

	XMINT3 ChunkManager::GetChunkIndex(XMVECTOR& position)
	{
		XMVECTOR index = position / ChunkSpecs.dimensions;
		return {
			(INT)(index.m128_f32[0]),
			(INT)(index.m128_f32[1]),
			(INT)(index.m128_f32[2])
		};
	}

	XMINT3 ChunkManager::GetBlockIndex(Chunk* pChunk, XMVECTOR& position)
	{
		XMVECTOR index = (position - pChunk->origin) / BlockSpecs.size;

		return {
			(INT)(index.m128_f32[0]),
			(INT)(index.m128_f32[1]),
			(INT)(index.m128_f32[2])
		};
	}

	bool ChunkManager::IsPositionInWorld(DirectX::XMVECTOR& position)
	{
		return position.m128_f32[0] >= 0.0f &&
			position.m128_f32[1] >= 0.0f &&
			position.m128_f32[2] >= 0.0f&&
			position.m128_f32[0] <= WorldSpecs.dimensions.x &&
			position.m128_f32[1] <= WorldSpecs.dimensions.y &&
			position.m128_f32[2] <= WorldSpecs.dimensions.z;
	}

	void ChunkManager::OnImgui()
	{
		if (ImGui::CollapsingHeader("Specs"))
		{
			ImGui::Text("ChunkSpecs");
			ImGui::Text("Wide %i", ChunkSpecs.wide);
			ImGui::Text("High %i", ChunkSpecs.high);
			ImGui::Text("Deep %i", ChunkSpecs.deep);
			ImGui::Text("Total blocks %i", ChunkSpecs.blocksTotal);
			ImGui::Text("Block stride %i", ChunkSpecs.blocksStride);
			ImGui::Text("Block slice %i", ChunkSpecs.blocksSlice);
			//ImGui::Text(" %i", ChunkSpecs.dimensions	  );
			ImGui::Text("WorldSpecs");
			ImGui::Text("Wide %i", WorldSpecs.wide);
			ImGui::Text("High %i", WorldSpecs.high);
			ImGui::Text("Deep %i", WorldSpecs.deep);
			//ImGui::Text(" %i", WorldSpecs.origin		  );
			ImGui::Text("Total chunks %i", WorldSpecs.chunksTotal);
			ImGui::Text("Chunk stride %i", WorldSpecs.chunksStride);
			ImGui::Text("Chunk slice %i", WorldSpecs.chunksSlice);
			ImGui::Text("Total blocks %i", WorldSpecs.blocksWorldTotal);
			ImGui::Text("Dimensions %.1f, %.1f, %.1f", WorldSpecs.dimensions.x, WorldSpecs.dimensions.y, WorldSpecs.dimensions.z);
		}
	}

	void ChunkManager::Initialise()
	{
		BlockSpecs.halfSize = BlockSpecs.size / 2.0f;

		ChunkSpecs.blocksStride = ChunkSpecs.wide;
		ChunkSpecs.blocksSlice = ChunkSpecs.blocksStride * ChunkSpecs.high;
		ChunkSpecs.blocksTotal = ChunkSpecs.blocksSlice * ChunkSpecs.deep;
		ChunkSpecs.dimensions = { BlockSpecs.size * ChunkSpecs.wide, BlockSpecs.size * ChunkSpecs.high, BlockSpecs.size * ChunkSpecs.deep, 0.0f };

		WorldSpecs.chunksStride = WorldSpecs.wide;
		WorldSpecs.chunksSlice = WorldSpecs.chunksStride * WorldSpecs.high;
		WorldSpecs.chunksTotal = WorldSpecs.chunksSlice * WorldSpecs.deep;
		WorldSpecs.blocksWorldTotal = WorldSpecs.chunksTotal * ChunkSpecs.blocksTotal;
		WorldSpecs.dimensions = { WorldSpecs.wide * ChunkSpecs.dimensions.m128_f32[0], WorldSpecs.high * ChunkSpecs.dimensions.m128_f32[1], WorldSpecs.deep * ChunkSpecs.dimensions.m128_f32[2]};

		worldBounds.Extents.x = WorldSpecs.dimensions.x / 2.0f;
		worldBounds.Extents.y = WorldSpecs.dimensions.y / 2.0f;
		worldBounds.Extents.z = WorldSpecs.dimensions.z / 2.0f;
		worldBounds.Center.x = worldBounds.Extents.x + WorldSpecs.origin.m128_f32[0];
		worldBounds.Center.y = worldBounds.Extents.y + WorldSpecs.origin.m128_f32[1];
		worldBounds.Center.z = worldBounds.Extents.z + WorldSpecs.origin.m128_f32[2];

		pChunksVector = std::make_unique<std::vector<Chunk>>(WorldSpecs.chunksTotal);
		pBlocksVector = std::make_unique<std::vector<Block>>(WorldSpecs.blocksWorldTotal);

		chunks = pChunksVector->data();
		blocks = pBlocksVector->data();
	}

	void ChunkManager::InitialiseChunks()
	{
		Chunk* pNextChunk = chunks;
		Block* pNextBlock = blocks;

		XMVECTOR chunkExtents = ChunkSpecs.dimensions / 2.0f;

		XMVECTOR chunkOrigin = WorldSpecs.origin;

		for (INT32 z = 0; z < WorldSpecs.deep; z++)
		{
			chunkOrigin.m128_f32[1] = WorldSpecs.origin.m128_f32[1];

			for (INT32 y = 0; y < WorldSpecs.high; y++)
			{
				chunkOrigin.m128_f32[0] = WorldSpecs.origin.m128_f32[0];

				for (INT32 x = 0; x < WorldSpecs.wide; x++)
				{
					pNextChunk->index = { x, y, z };
					pNextChunk->blocks = pNextBlock;
					pNextChunk->origin = chunkOrigin;
					XMStoreFloat3(&pNextChunk->bounds.Center, chunkOrigin + chunkExtents);
					XMStoreFloat3(&pNextChunk->bounds.Extents, chunkExtents);

					InitialiseBlocks(pNextChunk);

					pNextBlock = pNextChunk->blocks + ChunkSpecs.blocksTotal;
					pNextChunk++;

					chunkOrigin.m128_f32[0] += ChunkSpecs.dimensions.m128_f32[0];
				}

				chunkOrigin.m128_f32[1] += ChunkSpecs.dimensions.m128_f32[1];
			}

			chunkOrigin.m128_f32[2] += ChunkSpecs.dimensions.m128_f32[2];
		}
	}

	void ChunkManager::InitialiseBlocks(Chunk* pChunk)
	{
		Util::Random rng;

		Block* pNextBlock = pChunk->blocks;

		XMVECTOR blockOrigin = pChunk->origin;

		for (INT32 z = 0; z < ChunkSpecs.deep; z++)
		{
			blockOrigin.m128_f32[1] = pChunk->origin.m128_f32[1];

			for (INT32 y = 0; y < ChunkSpecs.high; y++)
			{
				blockOrigin.m128_f32[0] = pChunk->origin.m128_f32[0];

				for (INT32 x = 0; x < ChunkSpecs.wide; x++)
				{
					blockOrigin.m128_f32[0] += BlockSpecs.size;

					float typeRng = rng.Next();
					if (typeRng < 0.35f) pNextBlock->type = BlockType::BlockTypeGrass;
					else if (typeRng < 0.7f) pNextBlock->type = BlockType::BlockTypeDirt;
					else pNextBlock->type = BlockType::BlockTypeEmpty;

					pNextBlock++;
				}

				blockOrigin.m128_f32[1] += BlockSpecs.size;
			}

			blockOrigin.m128_f32[2] += BlockSpecs.size;
		}
	}

	void ChunkManager::InitialiseBlockFaces(Chunk* pChunk)
	{
		Block* pBlock = pChunk->blocks;

		for (INT32 z = 0; z < ChunkSpecs.deep; z++)
		{
			for (INT32 y = 0; y < ChunkSpecs.high; y++)
			{
				for (INT32 x = 0; x < ChunkSpecs.wide; x++)
				{
					if (pBlock->type != BlockType::BlockTypeEmpty)
					{
						// check if...
						//  at the edge of the chunk
						//  and chunks is at the edge of the world
						//  or no neighbour

						int mask = FaceMask::FaceMaskNone;

						if (x == 0)
						{
							if (pChunk->index.x == 0) mask |= FaceMask::FaceMaskLeft;
							else {
								Block* pBlockNeighbour = GetBlock(GetRelativeChunk(pChunk, -1, 0, 0), ChunkSpecs.wide - 1, y, z);
								if (pBlockNeighbour->type == BlockType::BlockTypeEmpty) mask |= FaceMask::FaceMaskLeft;
							}
						}
						else if ((pBlock - 1)->type == BlockType::BlockTypeEmpty) mask |= FaceMask::FaceMaskLeft;
							
						if (x == ChunkSpecs.wide - 1)
						{
							if (pChunk->index.x == WorldSpecs.wide - 1) mask |= FaceMask::FaceMaskRight;
							else {
								Block* pBlockNeighbour = GetBlock(GetRelativeChunk(pChunk, 1, 0, 0), 0, y, z);
								if (pBlockNeighbour->type == BlockType::BlockTypeEmpty) mask |= FaceMask::FaceMaskRight;
							}
						}
						else if ((pBlock + 1)->type == BlockType::BlockTypeEmpty) mask |= FaceMask::FaceMaskRight;

						if (y == 0)
						{
							if (pChunk->index.y == 0) mask |= FaceMask::FaceMaskBottom;
							else {
								Block* pBlockNeighbour = GetBlock(GetRelativeChunk(pChunk, 0, -1, 0), x, ChunkSpecs.high - 1, z);
								if (pBlockNeighbour->type == BlockType::BlockTypeEmpty) mask |= FaceMask::FaceMaskBottom;
							}
						}
						else if ((pBlock - ChunkSpecs.blocksStride)->type == BlockType::BlockTypeEmpty) mask |= FaceMask::FaceMaskBottom;

						if (y == ChunkSpecs.high - 1)
						{
							if (pChunk->index.y == WorldSpecs.high - 1) mask |= FaceMask::FaceMaskTop;
							else {
								Block* pBlockNeighbour = GetBlock(GetRelativeChunk(pChunk, 0, 1, 0), x, 0, z);
								if (pBlockNeighbour->type == BlockType::BlockTypeEmpty) mask |= FaceMask::FaceMaskTop;
							}
						}
						else if ((pBlock + ChunkSpecs.blocksStride)->type == BlockType::BlockTypeEmpty) mask |= FaceMask::FaceMaskTop;

						if (z == 0)
						{
							if (pChunk->index.z == 0) mask |= FaceMask::FaceMaskFront;
							else {
								Block* pBlockNeighbour = GetBlock(GetRelativeChunk(pChunk, 0, 0, -1), x, y, ChunkSpecs.deep - 1);
								if (pBlockNeighbour->type == BlockType::BlockTypeEmpty) mask |= FaceMask::FaceMaskFront;
							}
						}
						else if ((pBlock - ChunkSpecs.blocksSlice)->type == BlockType::BlockTypeEmpty) mask |= FaceMask::FaceMaskFront;

						if (z == ChunkSpecs.deep - 1)
						{
							if (pChunk->index.z == WorldSpecs.deep - 1) mask |= FaceMask::FaceMaskBack;
							else {
								Block* pBlockNeighbour = GetBlock(GetRelativeChunk(pChunk, 0, 0, 1), x, y, 0);
								if (pBlockNeighbour->type == BlockType::BlockTypeEmpty) mask |= FaceMask::FaceMaskBack;
							}
						}
						else if ((pBlock + ChunkSpecs.blocksSlice)->type == BlockType::BlockTypeEmpty) mask |= FaceMask::FaceMaskBack;

						pBlock->faceMask = (FaceMask)mask;
					}
					pBlock++;
				}
			}
		}
	}

	void ChunkManager::SetVertexBuffer(Chunk* pChunk)
	{
		long facecount = 0;
		std::vector<VertexData::Vertex> verticies;
		std::vector<UINT32> indicies;
		UINT32 indexCounter = 0, vertexCounter = 0;

		auto PushVertex = [&](VertexData::Vertex& v, XMVECTOR pos) {
			XMStoreFloat3(&v.position, pos);
			verticies.push_back(v);
		};

		auto PushIndicies = [&]() {
			indicies.push_back(vertexCounter);
			indicies.push_back(vertexCounter + 1);
			indicies.push_back(vertexCounter + 2);
			indicies.push_back(vertexCounter);
			indicies.push_back(vertexCounter + 2);
			indicies.push_back(vertexCounter + 3);
			vertexCounter += 4;
			facecount++;
		};

		Block* pBlock = pChunk->blocks;
		VertexData::Vertex v;
		XMVECTOR blockOrigin = pChunk->origin;

		for (INT32 z = 0; z < ChunkSpecs.deep; z++)
		{
			blockOrigin.m128_f32[1] = pChunk->origin.m128_f32[1];

			for (INT32 y = 0; y < ChunkSpecs.high; y++)
			{
				blockOrigin.m128_f32[0] = pChunk->origin.m128_f32[0];

				for (INT32 x = 0; x < ChunkSpecs.wide; x++)
				{
					if (pBlock->type != BlockType::BlockTypeEmpty)
					{
						v.blockType = pBlock->type;

						if (pBlock->faceMask & FaceMask::FaceMaskFront)
						{
							v.normal = { 0.0f, 0.0f, -1.0f };
							PushVertex(v, blockOrigin + XMVECTOR({ 0.0f, 0.0f, 0.0f, 1.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 0.0f, 1.0f, 0.0f, 1.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 1.0f, 1.0f, 0.0f, 1.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 1.0f, 0.0f, 0.0f, 1.0f }));
							PushIndicies();
						}
						if (pBlock->faceMask & FaceMask::FaceMaskRight)
						{
							v.normal = { 1.0f, 0.0f, 0.0f };
							PushVertex(v, blockOrigin + XMVECTOR({ 1.0f, 0.0f, 0.0f, 1.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 1.0f, 1.0f, 0.0f, 1.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 1.0f, 1.0f, 1.0f, 1.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 1.0f, 0.0f, 1.0f, 1.0f }));
							PushIndicies();
						}
						if (pBlock->faceMask & FaceMask::FaceMaskBack)
						{
							v.normal = { 0.0f, 0.0f, 1.0f };
							PushVertex(v, blockOrigin + XMVECTOR({ 1.0f, 0.0f, 1.0f, 1.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 1.0f, 1.0f, 1.0f, 1.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 0.0f, 1.0f, 1.0f, 1.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 0.0f, 0.0f, 1.0f, 1.0f }));
							PushIndicies();
						}
						if (pBlock->faceMask & FaceMask::FaceMaskLeft)
						{
							v.normal = { -1.0f, 0.0f, 0.0f };
							PushVertex(v, blockOrigin + XMVECTOR({ 0.0f, 0.0f, 1.0f, 1.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 0.0f, 1.0f, 1.0f, 1.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 0.0f, 1.0f, 0.0f, 1.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 0.0f, 0.0f, 0.0f, 1.0f }));
							PushIndicies();
						}
						if (pBlock->faceMask & FaceMask::FaceMaskTop)
						{
							v.normal = { 0.0f, 1.0f, 0.0f };
							PushVertex(v, blockOrigin + XMVECTOR({ 0.0f, 1.0f, 0.0f, 0.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 0.0f, 1.0f, 1.0f, 0.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 1.0f, 1.0f, 1.0f, 0.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 1.0f, 1.0f, 0.0f, 0.0f }));
							PushIndicies();
						}
						if (pBlock->faceMask & FaceMask::FaceMaskBottom)
						{
							v.normal = { 0.0f, -1.0f, 0.0f };
							PushVertex(v, blockOrigin + XMVECTOR({ 0.0f, 0.0f, 1.0f, 1.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 0.0f, 0.0f, 0.0f, 1.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 1.0f, 0.0f, 0.0f, 1.0f }));
							PushVertex(v, blockOrigin + XMVECTOR({ 1.0f, 0.0f, 1.0f, 1.0f }));
							PushIndicies();
						}
					}

					pBlock++;

					blockOrigin.m128_f32[0] += BlockSpecs.size;
				}

				blockOrigin.m128_f32[1] += BlockSpecs.size;
			}

			blockOrigin.m128_f32[2] += BlockSpecs.size;
		}

		if (verticies.size() > 0)
		{
			pChunk->pVB = std::make_shared<VertexBufferIndex>();
			pChunk->pVB->Initialise<VertexData::Vertex>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, verticies, indicies);
		}
	}

}
