#pragma once
#include "Common.h"
#include "Layer.h"
#include "Scene.h"
#include "Camera.h"

class BlockWorld : public Engine2::Layer
{
public:
	enum BlockState : unsigned char {
		empty, ground,
	};

	struct Block {
		unsigned char state;
	};

	BlockWorld();
	~BlockWorld();

	void OnUpdate(float dt);
	void OnRender();
	void OnImgui();

	bool RayHit(Engine2::Ray& ray, float** distance, Block** block);

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
		DirectX::XMVECTOR location;
	};
	std::vector<InstanceInfo> instances;
	UINT32 instanceCount;
	void UpdateInstances();

	std::shared_ptr<Engine2::VertexShaderDynamic> pVS;
	std::shared_ptr<Engine2::PixelShaderDynamic> pPS;
	std::shared_ptr<Engine2::DrawableInstanced> pVB;
	void CreateVertexBuffer();
	
	// temp
	Engine2::Ray ray;
	bool hit = false;
	float hitDistance;
	DirectX::XMVECTOR hitLocation;
};
