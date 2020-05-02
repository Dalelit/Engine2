#include "pch.h"
#include "Gizmos.h"
#include "Common.h"

//#define MAXVERTICIES 20000
//#define E2_GIZMO_CHECK(num) E2_ASSERT(vertexCount + num < MAXVERTICIES, "Exceeded gizmos line buffer");

#define E2_GIZMOZ_MAXINSTANCES 1000
#define E2_GIZMOZ_CHECKINSTANCES(num, instBuffer) E2_ASSERT(num + 1 <= instBuffer.size(), "Exceeded gizmoz buffer size");

using namespace DirectX;

namespace Engine2
{

	Gizmos::Gizmos() :
		psCB(0),
		axisVBuffer(AxisVerticies, AxisIndicies),
		sphereVBuffer(SphereVerticies, SphereIndicies),
		cameraVBuffer(CameraVerticies, CameraIndicies)
	{
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

		axisInstances.resize(E2_GIZMOZ_MAXINSTANCES);
		axisPtrInstancesBuffer = axisVBuffer.AddInstances(axisInstances, true);

		sphereInstances.resize(E2_GIZMOZ_MAXINSTANCES);
		spherePtrInstancesBuffer = sphereVBuffer.AddInstances(sphereInstances, true);

		cameraInstances.resize(E2_GIZMOZ_MAXINSTANCES); // to do: make sense having this many?
		cameraPtrInstancesBuffer = cameraVBuffer.AddInstances(cameraInstances, true);
	}

	void Gizmos::NewFrame()
	{
		axisInstanceCount = 0;
		sphereInstanceCount = 0;
		cameraInstanceCount = 0;
	}

	void Gizmos::Render()
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

	void Gizmos::UpdateBuffers()
	{
		if (axisInstanceCount > 0)
		{
			DXDevice::UpdateBuffer(axisPtrInstancesBuffer, axisInstances, axisInstanceCount);
			axisVBuffer.SetInstanceCount(axisInstanceCount);
		}

		if (sphereInstanceCount > 0)
		{
			DXDevice::UpdateBuffer(spherePtrInstancesBuffer, sphereInstances, sphereInstanceCount);
			sphereVBuffer.SetInstanceCount(sphereInstanceCount);
		}

		if (cameraInstanceCount > 0)
		{
			DXDevice::UpdateBuffer(cameraPtrInstancesBuffer, cameraInstances, cameraInstanceCount);
			cameraVBuffer.SetInstanceCount(cameraInstanceCount);
		}
	}

	void Gizmos::Draw()
	{
		if (axisInstanceCount > 0)
		{
			axisVBuffer.Bind();
			axisVBuffer.Draw();
		}

		if (sphereInstanceCount > 0)
		{
			sphereVBuffer.Bind();
			sphereVBuffer.Draw();
		}

		if (cameraInstanceCount > 0)
		{
			cameraVBuffer.Bind();
			cameraVBuffer.Draw();
		}
	}

	void Gizmos::OnImgui()
	{
		if (ImGui::TreeNode("Gizmos"))
		{
			ImGui::Checkbox("Active", &active);
			if (active)
			{
				ImGui::ColorEdit4("Visible", visibleColor.m128_f32);
				ImGui::ColorEdit4("Hidden", hiddenColor.m128_f32);
				ImGui::Text("Axis   %i", axisInstanceCount);
				ImGui::Text("Sphere %i", sphereInstanceCount);
				ImGui::Text("Camera %i", cameraInstanceCount);
			}
			ImGui::TreePop();
		}
	}

	void Gizmos::DrawAxis(DirectX::XMMATRIX instance)
	{
		E2_GIZMOZ_CHECKINSTANCES(axisInstanceCount, axisInstances);

		axisInstances[axisInstanceCount++] = XMMatrixTranspose(instance);
	}

	void Gizmos::DrawSphere(DirectX::XMMATRIX instance)
	{
		E2_GIZMOZ_CHECKINSTANCES(sphereInstanceCount, sphereInstances);

		sphereInstances[sphereInstanceCount++] = XMMatrixTranspose(instance);
	}

	void Gizmos::DrawCamera(DirectX::XMMATRIX instance)
	{
		E2_GIZMOZ_CHECKINSTANCES(cameraInstanceCount, cameraInstances);

		cameraInstances[cameraInstanceCount++] = XMMatrixTranspose(instance);
	}

	std::vector<XMFLOAT3> Gizmos::AxisVerticies = {
		{0.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
	};

	std::vector<unsigned int> Gizmos::AxisIndicies = {
		0,1, 0,2, 0,3
	};

	std::vector<XMFLOAT3> Gizmos::SphereVerticies = {
		{0.500f, 0.0f, 0.000f},
		{0.433f, 0.0f, 0.250f},
		{0.250f, 0.0f, 0.433f},
		{0.000f, 0.0f, 0.500f},
		{-0.250f, 0.0f, 0.433f},
		{-0.433f, 0.0f, 0.250f},
		{-0.500f, 0.0f, 0.000f},
		{-0.433f, 0.0f, -0.250f},
		{-0.250f, 0.0f, -0.433f},
		{0.000f, 0.0f, -0.500f},
		{0.250f, 0.0f, -0.433f},
		{0.433f, 0.0f, -0.250f},

		{0.0f, 0.000f, 0.500f},
		{0.0f, 0.250f, 0.433f},
		{0.0f, 0.433f, 0.250f},
		{0.0f, 0.500f, 0.000f},
		{0.0f, 0.433f, -0.250f},
		{0.0f, 0.250f, -0.433f},
		{0.0f, 0.000f, -0.500f},
		{0.0f, -0.250f, -0.433f},
		{0.0f, -0.433f, -0.250f},
		{0.0f, -0.500f, 0.000f},
		{0.0f, -0.433f, 0.250f},
		{0.0f, -0.250f, 0.433f},
	};

	std::vector<unsigned int> Gizmos::SphereIndicies = {
		0,1, 1,2, 2,3, 3,4, 4,5, 5,6, 6,7, 7,8, 8,9, 9,10, 10,11, 11,0,
		12,13, 13,14, 14,15, 15,16, 16,17, 17,18, 18,19, 19,20, 20,21, 21,22, 22,23, 23,12,
	};

	std::vector<XMFLOAT3> Gizmos::CameraVerticies = {
		{  0.0f,  0.0f,  0.0f},
		{  0.5f, -0.28f, 1.0f},
		{  0.5f,  0.28f, 1.0f},
		{ -0.5f,  0.28f, 1.0f},
		{ -0.5f, -0.28f, 1.0f},
		{ -0.4f,  0.3f,  1.0f},
		{  0.0f,  0.4f,  1.0f},
		{  0.4f,  0.3f,  1.0f},
	};

	std::vector<unsigned int> Gizmos::CameraIndicies = {
		0,1, 0,2, 0,3, 0,4,
		1,2, 2,3, 3,4, 4,1,
		5,6, 6,7, 7,5
	};
}
