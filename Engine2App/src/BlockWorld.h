#pragma once
#include "Common.h"
#include "Layer.h"
#include "Scene.h"
#include "Camera.h"

class BlockWorld : public Engine2::Layer
{
public:
	enum BlockType : unsigned char {
		empty, ground, grass
	};

	struct Block {
		BlockType type;
	};

	BlockWorld();
	~BlockWorld();

	void OnUpdate(float dt);
	void OnRender();
	void OnInputEvent(Engine2::InputEvent& event);
	void OnImgui();

	Block* RayHit(Engine2::Ray& ray, float& distance);

protected:
	Engine2::Scene scene;
	void MouseButtonReleased(Engine2::MouseButtonReleasedEvent& event);

	struct {
		INT32 wide = 10;
		INT32 high = 10;
		INT32 deep = 10;
		INT32 total, stride, slice;
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
	DirectX::XMVECTOR hitLocation;
	DirectX::XMFLOAT3 hitBlockCentre;
	DirectX::XMINT3   hitBlockIndx;
	DirectX::XMINT3   nextBlockIndx;

	void SetNextBlockIndx();
	void AddNextBlock();
	Block* GetBlock(DirectX::XMINT3& index);

	// stats
	unsigned long instanceUpdateCount = 0;
};
