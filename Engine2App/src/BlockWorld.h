#pragma once
#include "Common.h"
#include "Layer.h"
#include "Scene.h"
#include "Camera.h"
#include "Offscreen.h"
#include "BlockWorldChunk.h"

namespace BlockWorld
{

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
		void OnApplicationEvent(Engine2::ApplicationEvent& event);
		void OnImgui();
		void OnGizmos();

		Block* RayHit(Engine2::Ray& ray, float& distance);

	protected:
		Engine2::Scene scene;
		void MouseButtonReleased(Engine2::MouseButtonReleasedEvent& event);
		void KeyPressed(Engine2::KeyPressedEvent& event);

		//struct {
		//	INT32 wide = 10;
		//	INT32 high = 10;
		//	INT32 deep = 10;
		//	INT32 total, stride, slice;
		//	float cubeSize = 1.0f;
		//	DirectX::BoundingBox bounds;
		//} segment;

		//std::vector<Block> blocks;

		//void InitialiseBlocks();

		//struct InstanceInfo {
		//	DirectX::XMFLOAT3 location;
		//	UINT32 type;
		//};
		//bool instancesDirty = true; // flag to update the buffer
		//std::vector<InstanceInfo> instances;
		//UINT32 instanceCount;
		//void UpdateInstances();

		//std::shared_ptr<Engine2::VertexShaderDynamic> pVS;
		//std::shared_ptr<Engine2::PixelShaderDynamic> pPS;
		//std::shared_ptr<Engine2::DrawableInstanced> pVB;
		//ID3D11Buffer* instanceBuffer;
		//void CreateVertexBuffer();

		//struct {
		//	std::shared_ptr<Engine2::VertexShaderDynamic> pVS;
		//	std::shared_ptr<Engine2::PixelShaderDynamic> pPS;
		//	std::shared_ptr<Engine2::Drawable> pVB;
		//	std::shared_ptr<Engine2::VSConstantBuffer<DirectX::XMVECTOR>> pVSCB;
		//	std::shared_ptr<Engine2::PSConstantBuffer<DirectX::XMVECTOR>> pPSCB;
		//	wrl::ComPtr<ID3D11DepthStencilState> pBackDrawDSS;
		//	DirectX::XMVECTOR color = { 1.0f, 1.0f, 0.0f, 1.0f };
		//	DirectX::XMVECTOR hiddenColor = { 0.3f, 0.3f, 0.0f, 1.0f };
		//	DirectX::XMVECTOR nextColor = { 0.8f, 0.8f, 0.8f, 1.0f };
		//} highlighter;
		//void CreateVertexBufferHighlight();

		// manage the selected block
		//float hitDistance;
		//Block* pHitBlock = nullptr;
		//DirectX::XMVECTOR hitLocation;
		//DirectX::XMFLOAT3 hitBlockCentre;
		//DirectX::XMINT3   hitBlockIndx;
		//DirectX::XMINT3   nextBlockIndx;
		//DirectX::XMFLOAT3 nextBlockCentre;

		//void SetNextBlockIndx();
		//void AddNextBlock();
		//void RemoveBlock();
		//Block* GetBlock(DirectX::XMINT3& index);

		// stats
		//unsigned long instanceUpdateCount = 0;

		Engine2::OffscreenWithDepthBuffer offscreen;

		ChunkManager chunkMgr;
		bool blockHit = false;
		HitInfo hitInfo;
		DirectX::XMVECTOR hitLocation;

		bool wireframeOn = false;
	};

}
