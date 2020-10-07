#include "pch.h"
#include "GizmoRender.h"
#include "Common.h"

using namespace DirectX;

namespace Engine2
{
	GizmoRender::GizmoRender(size_t maxGizmos) :
		maxGizmos(maxGizmos), psCB(0)
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
		E2_ASSERT(axisInstances.size() < maxGizmos, "Exceed gizmo count");
		axisInstances.emplace_back(instance);
	}

	void GizmoRender::DrawSphere(DirectX::XMMATRIX instance)
	{
		E2_ASSERT(sphereInstances.size() < maxGizmos, "Exceed gizmo count");
		sphereInstances.emplace_back(instance);
	}

	void GizmoRender::DrawCube(DirectX::XMMATRIX instance)
	{
		E2_ASSERT(cubeInstances.size() < maxGizmos, "Exceed gizmo count");
		cubeInstances.emplace_back(instance);
	}

	void GizmoRender::DrawCamera(DirectX::XMMATRIX instance)
	{
		E2_ASSERT(cameraInstances.size() < maxGizmos, "Exceed gizmo count");
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

			axisVBuffer.Initialise<XMFLOAT3>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
				AxisVerticies, AxisIndicies);
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

			sphereVBuffer.Initialise<XMFLOAT3>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
				SphereVerticies, SphereIndicies);
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

			cubeVBuffer.Initialise<XMFLOAT3>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
				CubeVerticies, CubeIndicies);
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

			cameraVBuffer.Initialise<XMFLOAT3>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
				CameraVerticies, CameraIndicies);


			// set the instance buffers on them
			axisVBuffer.SetInstances<InstanceInfoType>(maxGizmos);
			cubeVBuffer.SetInstances<InstanceInfoType>(maxGizmos);
			sphereVBuffer.SetInstances<InstanceInfoType>(maxGizmos);
			cameraVBuffer.SetInstances<InstanceInfoType>(maxGizmos);
		}
	}

	void GizmoRender::Draw()
	{
		if (axisInstances.size() > 0)
		{
			axisVBuffer.UpdateInstanceBuffer(axisInstances, (UINT)axisInstances.size());
			axisVBuffer.Bind();
			axisVBuffer.Draw((UINT)axisInstances.size());
		}

		if (sphereInstances.size() > 0)
		{
			sphereVBuffer.UpdateInstanceBuffer(sphereInstances, (UINT)sphereInstances.size());
			sphereVBuffer.Bind();
			sphereVBuffer.Draw((UINT)sphereInstances.size());
		}

		if (cubeInstances.size() > 0)
		{
			cubeVBuffer.UpdateInstanceBuffer(cubeInstances, (UINT)cubeInstances.size());
			cubeVBuffer.Bind();
			cubeVBuffer.Draw((UINT)cubeInstances.size());
		}

		if (cameraInstances.size() > 0)
		{
			cameraVBuffer.UpdateInstanceBuffer(cameraInstances, (UINT)cameraInstances.size());
			cameraVBuffer.Bind();
			cameraVBuffer.Draw((UINT)cameraInstances.size());
		}
	}

}
