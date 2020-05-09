#pragma once
#include "Common.h"
#include "Layer.h"
#include "Scene.h"
#include "Camera.h"

class BlockWorld : public Engine2::Layer
{
public:
	enum BlockType : unsigned char {
		empty, selected, ground, grass
	};

	struct Block {
		BlockType type;
	};

	BlockWorld();
	~BlockWorld();

	void OnUpdate(float dt);
	void OnRender();
	void OnImgui();

	Block* RayHit(Engine2::Ray& ray, float& distance);

protected:
	Engine2::Scene scene;

	struct {
		UINT32 wide = 10;
		UINT32 high = 10;
		UINT32 deep = 10;
		UINT32 total, stride, slice;
		float cubeSize = 1.0f;
		DirectX::BoundingBox bounds;
	} segment;

	std::vector<Block> blocks;

	void InitialiseBlocks();

	struct InstanceInfo {
		DirectX::XMFLOAT3 location;
		UINT32 type;
	};
	bool instancesDirty = true; // flag to update the buffer
	std::vector<InstanceInfo> instances;
	UINT32 instanceCount;
	void UpdateInstances();

	std::shared_ptr<Engine2::VertexShaderDynamic> pVS;
	std::shared_ptr<Engine2::PixelShaderDynamic> pPS;
	std::shared_ptr<Engine2::DrawableInstanced> pVB;
	ID3D11Buffer* instanceBuffer;
	void CreateVertexBuffer();
	
	// manage the selected block
	float hitDistance;
	Block* pHitBlock = nullptr;
	BlockType hitBlockType;

	// stats
	unsigned long instanceUpdateCount = 0;
};
