#pragma once
#include "pch.h"
#include "VertexBuffer.h"
#include "VertexBufferInstanced.h"
#include "Shader.h"
#include "ConstantBuffer.h"

namespace Engine2
{
	struct Gizmo
	{
		enum Types { Axis, Cube, Sphere, Camera };

		Gizmo() : type(Types::Sphere) {}
		Gizmo(Types type) : type(type) {}

		Types type;

		void OnImgui();
	};

	class GizmoRender
	{
	public:

		GizmoRender(size_t maxGizmos = 50);
		~GizmoRender() = default;

		void NewFrame();
		void Render();

		void DrawAxis(const DirectX::XMMATRIX& instance);
		void DrawSphere(const DirectX::XMMATRIX& instance);
		void DrawCube(const DirectX::XMMATRIX& instance);
		void DrawCamera(const DirectX::XMMATRIX& instance, float nearPlane, float farPlane, float aspectRatio, float fieldOfView);

	protected:
		size_t maxGizmos;
		std::shared_ptr<PixelShader> pPS;
		std::shared_ptr<VertexShader> pVS;
		PSConstantBuffer<DirectX::XMVECTOR> psCB;
		DirectX::XMVECTOR visibleColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMVECTOR hiddenColor = { 0.3f, 0.3f, 0.3f, 1.0f };
		wrl::ComPtr<ID3D11DepthStencilState> pBackDrawDSS;
		using InstanceInfoType = DirectX::XMMATRIX;

		struct Vertex {
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 color;
		};

		void CreateBuffers();

		void Draw();

		// sphere
		std::vector<DirectX::XMVECTOR> spherePositions; // pre calculated
		UINT pointsPerCircle = 24u;

		// line buffer
		std::vector<Vertex> lineBuffer;
		VertexBuffer lineVBuffer;


		constexpr static DirectX::XMFLOAT3 xAxisColor = { 1.0f, 0.0f, 0.0f };
		constexpr static DirectX::XMFLOAT3 yAxisColor = { 0.0f, 1.0f, 0.0f };
		constexpr static DirectX::XMFLOAT3 zAxisColor = { 0.0f, 0.0f, 1.0f };
		constexpr static DirectX::XMFLOAT3 cubeColor = { 1.0f, 1.0f, 1.0f };
		constexpr static DirectX::XMFLOAT3 cameraColor = { 1.0f, 0.0f, 1.0f };
	};
}