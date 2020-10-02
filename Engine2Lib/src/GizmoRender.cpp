#include "pch.h"
#include "GizmoRender.h"

// To Do's:
// - use a single instance buffer
// - work out the max gizmo size and do checking

using namespace DirectX;

namespace Engine2
{
	GizmoRender::GizmoRender() :
		psCB(0)
	{
		CreateVertexBuffers();

		std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
			{"Position",      0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"InstTransform", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"InstTransform", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"InstTransform", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"InstTransform", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		};

		std::string vsFileName = Config::directories["ShaderCompiledDir"] + "GizmosVS.cso";
		pVS = VertexShader::CreateFromCompiledFile(vsFileName, vsLayout);

		std::string psFileName = Config::directories["ShaderCompiledDir"] + "GizmosPS.cso";
		pPS = PixelShader::CreateFromCompiledFile(psFileName);

		D3D11_DEPTH_STENCIL_DESC desc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER;
		DXDevice::GetDevice().CreateDepthStencilState(&desc, &pBackDrawDSS);

		// To Do: hack. see notes for fixes
		axisInstances.resize(50);
		sphereInstances.resize(50);
		cubeInstances.resize(50);
		cameraInstances.resize(50);

		axisPtrInstancesBuffer = axisVBuffer->AddInstances(axisInstances, true);
		spherePtrInstancesBuffer = sphereVBuffer->AddInstances(sphereInstances, true);
		cubePtrInstancesBuffer = cubeVBuffer->AddInstances(cubeInstances, true);
		cameraPtrInstancesBuffer = cameraVBuffer->AddInstances(cameraInstances, true);
	}

	void GizmoRender::NewFrame()
	{
		axisInstances.clear();
		sphereInstances.clear();
		cubeInstances.clear();
		cameraInstances.clear();
	}

	void GizmoRender::Render()
	{
		UpdateBuffers();

		// Note: Assuming the scene has already bound the camera matrix to VS constant buffer 0
		pVS->Bind();
		pPS->Bind();

		psCB.data = visibleColor;
		psCB.Bind();
		Draw();

		DXDevice::GetContext().OMSetDepthStencilState(pBackDrawDSS.Get(), 0);
		psCB.data = hiddenColor;
		psCB.Bind();
		Draw();

		DXDevice::Get().SetDefaultDepthStencilState(); // revert back to default
	}

	void GizmoRender::DrawAxis(DirectX::XMMATRIX instance)
	{
		axisInstances.emplace_back(instance);
	}

	void GizmoRender::DrawSphere(DirectX::XMMATRIX instance)
	{
		sphereInstances.emplace_back(instance);
	}

	void GizmoRender::DrawCube(DirectX::XMMATRIX instance)
	{
		cubeInstances.emplace_back(instance);
	}

	void GizmoRender::DrawCamera(DirectX::XMMATRIX instance)
	{
		cameraInstances.emplace_back(instance);
	}

	void GizmoRender::CreateVertexBuffers()
	{
		{
			std::vector<XMFLOAT3> AxisVerticies = {
				{0.0f, 0.0f, 0.0f},
				{1.0f, 0.0f, 0.0f},
				{0.0f, 1.0f, 0.0f},
				{0.0f, 0.0f, 1.0f},
			};

			std::vector<uint32_t> AxisIndicies = {
				0,1, 0,2, 0,3
			};

			axisVBuffer = std::make_unique<GizmoVBType>(AxisVerticies, AxisIndicies);
		}
		{
			std::vector<XMFLOAT3> SphereVerticies = {
				{0.500f,  0.0f, 0.000f},
				{0.433f,  0.0f, 0.250f},
				{0.250f,  0.0f, 0.433f},
				{0.000f,  0.0f, 0.500f},
				{-0.250f, 0.0f, 0.433f},
				{-0.433f, 0.0f, 0.250f},
				{-0.500f, 0.0f, 0.000f},
				{-0.433f, 0.0f, -0.250f},
				{-0.250f, 0.0f, -0.433f},
				{0.000f,  0.0f, -0.500f},
				{0.250f,  0.0f, -0.433f},
				{0.433f,  0.0f, -0.250f},

				{0.0f,  0.000f, 0.500f},
				{0.0f,  0.250f, 0.433f},
				{0.0f,  0.433f, 0.250f},
				{0.0f,  0.500f, 0.000f},
				{0.0f,  0.433f, -0.250f},
				{0.0f,  0.250f, -0.433f},
				{0.0f,  0.000f, -0.500f},
				{0.0f, -0.250f, -0.433f},
				{0.0f, -0.433f, -0.250f},
				{0.0f, -0.500f, 0.000f},
				{0.0f, -0.433f, 0.250f},
				{0.0f, -0.250f, 0.433f},

				{0.000f,   0.500f, 0.0f},
				{0.250f,   0.433f, 0.0f},
				{0.433f,   0.250f, 0.0f},
				{0.500f,   0.000f, 0.0f},
				{0.433f,  -0.250f, 0.0f},
				{0.250f,  -0.433f, 0.0f},
				{0.000f,  -0.500f, 0.0f},
				{-0.250f, -0.433f, 0.0f},
				{-0.433f, -0.250f, 0.0f},
				{-0.500f,  0.000f, 0.0f},
				{-0.433f,  0.250f, 0.0f},
				{-0.250f,  0.433f, 0.0f},
			};

			std::vector<uint32_t> SphereIndicies = {
				0,1, 1,2, 2,3, 3,4, 4,5, 5,6, 6,7, 7,8, 8,9, 9,10, 10,11, 11,0,
				12,13, 13,14, 14,15, 15,16, 16,17, 17,18, 18,19, 19,20, 20,21, 21,22, 22,23, 23,12,
				24,25, 25,26, 26,27, 27,28, 28,29, 29,30, 30,31, 31,32, 32,33, 33,34, 34,35, 35,24,
			};

			sphereVBuffer = std::make_unique<GizmoVBType>(SphereVerticies, SphereIndicies);
		}
		{
			std::vector<XMFLOAT3> CubeVerticies = {
				{-0.5f, -0.5f, -0.5f},
				{-0.5f,  0.5f, -0.5f},
				{ 0.5f,  0.5f, -0.5f},
				{ 0.5f, -0.5f, -0.5f},
				{-0.5f, -0.5f,  0.5f},
				{-0.5f,  0.5f,  0.5f},
				{ 0.5f,  0.5f,  0.5f},
				{ 0.5f, -0.5f,  0.5f},
			};

			std::vector<uint32_t> CubeIndicies = {
				0,1, 1,2, 2,3 ,3,0,
				4,5, 5,6, 6,7, 7,4,
				0,4, 1,5, 2,6, 3,7,
			};

			cubeVBuffer = std::make_unique<GizmoVBType>(CubeVerticies, CubeIndicies);
		}
		{
			std::vector<XMFLOAT3> CameraVerticies = {
				{  0.0f,  0.0f,  0.0f},
				{  0.5f, -0.28f, 1.0f},
				{  0.5f,  0.28f, 1.0f},
				{ -0.5f,  0.28f, 1.0f},
				{ -0.5f, -0.28f, 1.0f},
				{ -0.4f,  0.3f,  1.0f},
				{  0.0f,  0.4f,  1.0f},
				{  0.4f,  0.3f,  1.0f},
			};

			std::vector<uint32_t> CameraIndicies = {
				0,1, 0,2, 0,3, 0,4,
				1,2, 2,3, 3,4, 4,1,
				5,6, 6,7, 7,5
			};

			cameraVBuffer = std::make_unique<GizmoVBType>(CameraVerticies, CameraIndicies);
		}
	}

	void GizmoRender::UpdateBuffers()
	{
		if (axisInstances.size() > 0)
		{
			DXDevice::UpdateBuffer(axisPtrInstancesBuffer, axisInstances, (UINT)axisInstances.size());
			axisVBuffer->SetInstanceCount((UINT)axisInstances.size());
		}

		if (sphereInstances.size() > 0)
		{
			DXDevice::UpdateBuffer(spherePtrInstancesBuffer, sphereInstances, (UINT)sphereInstances.size());
			sphereVBuffer->SetInstanceCount((UINT)sphereInstances.size());
		}

		if (cubeInstances.size() > 0)
		{
			DXDevice::UpdateBuffer(cubePtrInstancesBuffer, cubeInstances, (UINT)cubeInstances.size());
			cubeVBuffer->SetInstanceCount((UINT)cubeInstances.size());
		}

		if (cameraInstances.size() > 0)
		{
			DXDevice::UpdateBuffer(cameraPtrInstancesBuffer, cameraInstances, (UINT)cameraInstances.size());
			cameraVBuffer->SetInstanceCount((UINT)cameraInstances.size());
		}
	}

	void GizmoRender::Draw()
	{
		if (axisInstances.size() > 0) axisVBuffer->BindAndDraw();
		if (sphereInstances.size() > 0) sphereVBuffer->BindAndDraw();
		if (cubeInstances.size() > 0) cubeVBuffer->BindAndDraw();
		if (cameraInstances.size() > 0) cameraVBuffer->BindAndDraw();
	}

}
