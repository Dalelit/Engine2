#pragma once
#include "pch.h"
#include "VertexBufferInstanced.h"
#include "Shader.h"
#include "ConstantBuffer.h"

namespace Engine2
{
	class GizmoRender
	{
		using GizmoVBType = VertexBufferIndexInstanced<DirectX::XMFLOAT3, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST>;
	public:

		GizmoRender();
		~GizmoRender() = default;

		void NewFrame();
		void Render();

		void DrawAxis(DirectX::XMMATRIX instance);
		void DrawSphere(DirectX::XMMATRIX instance);
		void DrawCube(DirectX::XMMATRIX instance);
		void DrawCamera(DirectX::XMMATRIX instance);

	protected:
		std::shared_ptr<PixelShader> pPS;
		std::shared_ptr<VertexShader> pVS;
		PSConstantBuffer<DirectX::XMVECTOR> psCB;
		DirectX::XMVECTOR visibleColor = { 0.2f, 0.8f, 0.2f, 1.0f };
		DirectX::XMVECTOR hiddenColor = { 0.05f, 0.2f, 0.05f, 1.0f };
		wrl::ComPtr<ID3D11DepthStencilState> pBackDrawDSS;

		void CreateVertexBuffers();

		void UpdateBuffers();
		void Draw();

		// axis
		std::vector<DirectX::XMMATRIX> axisInstances;
		std::unique_ptr<GizmoVBType> axisVBuffer;
		ID3D11Buffer* axisPtrInstancesBuffer;

		// sphere
		std::vector<DirectX::XMMATRIX> sphereInstances;
		std::unique_ptr<GizmoVBType> sphereVBuffer;
		ID3D11Buffer* spherePtrInstancesBuffer;

		// cube
		std::vector<DirectX::XMMATRIX> cubeInstances;
		std::unique_ptr<GizmoVBType> cubeVBuffer;
		ID3D11Buffer* cubePtrInstancesBuffer;

		// camera
		std::vector<DirectX::XMMATRIX> cameraInstances;
		std::unique_ptr<GizmoVBType> cameraVBuffer;
		ID3D11Buffer* cameraPtrInstancesBuffer;
	};
}