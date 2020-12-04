#pragma once
#include "Common.h"
#include "VertexBuffer.h"
#include "Shader.h"

namespace BlockWorld
{
	enum FaceMask : char {
		FaceMaskNone   = 0b00000000,
		FaceMaskFront  = 0b00000001,
		FaceMaskRight  = 0b00000010,
		FaceMaskBack   = 0b00000100,
		FaceMaskLeft   = 0b00001000,
		FaceMaskTop    = 0b00010000,
		FaceMaskBottom = 0b00100000,
		FaceMaskAll    = 0b00111111,
	};

	enum BlockType : char {
		BlockTypeEmpty = 0,
		BlockTypeDirt  = 1,
		BlockTypeGrass = 2,
	};

	class VertexData
	{
	public:
		struct Vertex {
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
			UINT32 blockType;
		};

		std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
			{"Position", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"Normal", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BlockType", 0, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
	};

	struct Block
	{
		BlockType type;
		FaceMask faceMask;
	};

	struct Chunk
	{
		DirectX::XMINT3 index;
		DirectX::XMVECTOR origin;
		DirectX::BoundingBox bounds;
		Block* blocks;
		std::shared_ptr<Engine2::VertexBufferIndex> pVB;
	};

	struct HitInfo
	{
		Chunk* pChunk;
		Block* pBlock;
		DirectX::XMINT3 blockIndex;
		DirectX::XMFLOAT3 centre;
		float  distance;
	};

	class ChunkManager
	{
	public:
		ChunkManager();
		~ChunkManager();

		struct {
			float size = 1.0f;

			// calculated in initialise
			float halfSize;
		} BlockSpecs;

		struct {
			INT32 wide = 3;
			INT32 high = 3;
			INT32 deep = 3;
			
			// calculated in initialise
			INT32 blocksTotal, blocksStride, blocksSlice;
			DirectX::XMVECTOR dimensions;
		} ChunkSpecs;

		struct {
			INT32 wide = 12;
			INT32 high = 3;
			INT32 deep = 12;
			DirectX::XMVECTOR origin = { 0.0f, 0.0f, 0.0f, 0.0f };

			// calculated in initialise
			INT32 chunksTotal, chunksStride, chunksSlice, blocksWorldTotal;
			DirectX::XMFLOAT3 dimensions;
		} WorldSpecs;

		inline Chunk* GetChunk(INT32 x, INT32 y, INT32 z) { return ChunksBegin() + (z * WorldSpecs.chunksSlice + y * WorldSpecs.chunksStride + x); }
		inline Chunk* GetChunk(DirectX::XMINT3 index)     { return GetChunk(index.x, index.y, index.z); }
		
		inline Chunk* GetRelativeChunk(Chunk* pChunk, INT32 dx, INT32 dy, INT32 dz) { return ChunksBegin() + ((pChunk->index.z + dz) * WorldSpecs.chunksSlice + (pChunk->index.y + dy) * WorldSpecs.chunksStride + (pChunk->index.x + dx)); }
		
		inline Block* GetBlock(Chunk* pChunk, INT32 x, INT32 y, INT32 z) { return pChunk->blocks + (z * ChunkSpecs.blocksSlice + y * ChunkSpecs.blocksStride + x); }
		inline Block* GetBlock(Chunk* pChunk, DirectX::XMINT3 index)     { return GetBlock(pChunk, index.x, index.y, index.z); }

		DirectX::XMFLOAT3 GetBlockCentre(Chunk* pChunk, DirectX::XMINT3 index) { return { (float)index.x + BlockSpecs.halfSize + pChunk->origin.m128_f32[0], (float)index.y + BlockSpecs.halfSize + pChunk->origin.m128_f32[1], (float)index.z + BlockSpecs.halfSize + pChunk->origin.m128_f32[2] }; }

		inline std::vector<Chunk>& Chunks() { return *pChunksVector; }
		inline Chunk* ChunksBegin()         { return chunks; }
		inline Chunk* ChunksEnd()           { return chunks + WorldSpecs.chunksTotal; }

		bool Intersects(DirectX::XMVECTOR rayOrigin, DirectX::XMVECTOR rayDirection, HitInfo& hitInfo);

		void RemoveBlock(HitInfo& hitInfo);
		void AddBlock(HitInfo& hitInfo, DirectX::XMVECTOR& hitLocation);

		void GetNeighbours(HitInfo& hitInfo, std::vector<Chunk*>& neighbours);

		Block* GetBlockPrevX(Chunk* pChunk, DirectX::XMINT3& blockIndx);
		Block* GetBlockNextX(Chunk* pChunk, DirectX::XMINT3& blockIndx);
		Block* GetBlockPrevY(Chunk* pChunk, DirectX::XMINT3& blockIndx);
		Block* GetBlockNextY(Chunk* pChunk, DirectX::XMINT3& blockIndx);
		Block* GetBlockPrevZ(Chunk* pChunk, DirectX::XMINT3& blockIndx);
		Block* GetBlockNextZ(Chunk* pChunk, DirectX::XMINT3& blockIndx);

		DirectX::XMINT3 GetChunkIndex(DirectX::XMVECTOR& position);
		DirectX::XMINT3 GetBlockIndex(Chunk* pChunk, DirectX::XMVECTOR& position);
		
		bool IsPositionInWorld(DirectX::XMVECTOR& position);
		bool IsValidChunkIndex(DirectX::XMINT3 index) { return index.x >= 0 && index.y >= 0 && index.z >= 0 && index.x < WorldSpecs.wide && index.y < WorldSpecs.high && index.z < WorldSpecs.deep; }
		bool IsValidBlockIndex(DirectX::XMINT3 index) { return index.x >= 0 && index.y >= 0 && index.z >= 0 && index.x < ChunkSpecs.wide && index.y < ChunkSpecs.high && index.z < ChunkSpecs.deep; }


		std::shared_ptr<Engine2::VertexShaderFile> pVS;
		std::shared_ptr<Engine2::PixelShaderFile>  pPS;

		void OnImgui();

	protected:
		Chunk* chunks = nullptr;
		Block* blocks = nullptr;
		std::unique_ptr<std::vector<Chunk>> pChunksVector;
		std::unique_ptr<std::vector<Block>> pBlocksVector;

		void Initialise();
		void InitialiseChunks();
		void InitialiseBlocks(Chunk* pChunk);
		void InitialiseBlockFaces(Chunk* pChunk);
		void SetVertexBuffer(Chunk* pChunk);

		DirectX::BoundingBox worldBounds;
	};
}
