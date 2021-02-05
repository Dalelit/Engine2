#include "pch.h"
#include "Gizmo.h"
#include "Common.h"

using namespace DirectX;

namespace Engine2
{
	void Gizmo::OnImgui()
	{
		const char* selected = "";

		switch (type)
		{
		case Types::Axis: selected = "Axis"; break;
		case Types::Cube: selected = "Cube"; break;
		case Types::Sphere: selected = "Sphere"; break;
		case Types::Camera: selected = "Camera"; break;
		}

		if (ImGui::BeginCombo("Type", selected))
		{
			if (ImGui::Selectable("Axis", type == Types::Axis)) type = Types::Axis;
			if (ImGui::Selectable("Cube", type == Types::Cube)) type = Types::Cube;
			if (ImGui::Selectable("Sphere", type == Types::Sphere)) type = Types::Sphere;
			if (ImGui::Selectable("Camera", type == Types::Camera)) type = Types::Camera;
			ImGui::EndCombo();
		}
	}

	GizmoRender::GizmoRender(size_t maxGizmos) :
		maxGizmos(maxGizmos), psCB(2u)
	{
		CreateBuffers();

		std::vector<D3D11_INPUT_ELEMENT_DESC> vsLayout = {
			{"Position",      0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"Color",         0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		std::string vsFileName = Config::directories["ShaderCompiledDir"] + "GizmosVS.cso";
		pVS = VertexShader::CreateFromCompiledFile(vsFileName, vsLayout);

		std::string psFileName = Config::directories["ShaderCompiledDir"] + "GizmosPS.cso";
		pPS = PixelShader::CreateFromCompiledFile(psFileName);

		D3D11_DEPTH_STENCIL_DESC desc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
		DXDevice::GetDevice().CreateDepthStencilState(&desc, &pBackDrawDSS);
	}

	void GizmoRender::NewFrame()
	{
		lineBuffer.clear();
	}

	void GizmoRender::Render()
	{
		// Note: Assuming the scene has already bound the camera matrix to VS constant buffer 0
		pPS->Bind();

		// draw hidden lines

		DXDevice::GetContext().OMSetDepthStencilState(pBackDrawDSS.Get(), 0);
		psCB.data = hiddenColor;
		psCB.Bind();
		pVS->Bind();
		Draw();

		// draw visible lines

		DXDevice::Get().SetDefaultDepthStencilState(); // revert back to default

		psCB.data = visibleColor;
		psCB.Bind();
		pVS->Bind();
		Draw();

	}

	void GizmoRender::DrawAxis(const DirectX::XMMATRIX& transform)
	{
		Vertex v0, v1;

		XMStoreFloat3(&v0.position, XMVector3Transform({ 0.0f, 0.0f, 0.0f, 1.0f }, transform));

		// x axis
		v0.color = xAxisColor;
		v1.color = xAxisColor;
		XMStoreFloat3(&v1.position, XMVector3Transform({ 1.0f, 0.0f, 0.0f, 1.0f }, transform));
		lineBuffer.push_back(v0);
		lineBuffer.push_back(v1);

		// y axis
		v0.color = yAxisColor;
		v1.color = yAxisColor;
		XMStoreFloat3(&v1.position, XMVector3Transform({ 0.0f, 1.0f, 0.0f, 1.0f }, transform));
		lineBuffer.push_back(v0);
		lineBuffer.push_back(v1);

		// z axis
		v0.color = zAxisColor;
		v1.color = zAxisColor;
		XMStoreFloat3(&v1.position, XMVector3Transform({ 0.0f, 0.0f, 1.0f, 1.0f }, transform));
		lineBuffer.push_back(v0);
		lineBuffer.push_back(v1);
	}

	void GizmoRender::DrawSphere(const DirectX::XMMATRIX& transform, DirectX::XMFLOAT3 xColor, DirectX::XMFLOAT3 yColor, DirectX::XMFLOAT3 zColor)
	{
		auto position = spherePositions.begin();
		Vertex vStart, vCurrent, vPrevious;

		auto drawCircle = [&](XMFLOAT3 color) {
			vStart.color = color;
			vCurrent.color = color;
			XMStoreFloat3(&vStart.position, XMVector3Transform(*position, transform));
			vPrevious = vStart;
			++position;

			for (UINT i = 1; i < pointsPerCircle; ++i)
			{
				XMStoreFloat3(&vCurrent.position, XMVector3Transform(*position, transform));
				lineBuffer.push_back(vPrevious);
				lineBuffer.push_back(vCurrent);
				vPrevious = vCurrent;
				++position;
			}
			lineBuffer.push_back(vCurrent);
			lineBuffer.push_back(vStart);
		};

		drawCircle(xColor);
		drawCircle(yColor);
		drawCircle(zColor);
	}

	void GizmoRender::DrawCube(const DirectX::XMMATRIX& transform, DirectX::XMFLOAT3 color)
	{
		XMFLOAT3 positions[8];

		XMFLOAT3* pos = positions;
		XMStoreFloat3(pos, XMVector3Transform({ -0.5f,  -0.5f,  -0.5f, 1.0f }, transform)); ++pos;
		XMStoreFloat3(pos, XMVector3Transform({ -0.5f,   0.5f,  -0.5f, 1.0f }, transform)); ++pos;
		XMStoreFloat3(pos, XMVector3Transform({  0.5f,   0.5f,  -0.5f, 1.0f }, transform)); ++pos;
		XMStoreFloat3(pos, XMVector3Transform({  0.5f,  -0.5f,  -0.5f, 1.0f }, transform)); ++pos;
		XMStoreFloat3(pos, XMVector3Transform({ -0.5f,  -0.5f,   0.5f, 1.0f }, transform)); ++pos;
		XMStoreFloat3(pos, XMVector3Transform({ -0.5f,   0.5f,   0.5f, 1.0f }, transform)); ++pos;
		XMStoreFloat3(pos, XMVector3Transform({  0.5f,   0.5f,   0.5f, 1.0f }, transform)); ++pos;
		XMStoreFloat3(pos, XMVector3Transform({  0.5f,  -0.5f,   0.5f, 1.0f }, transform));

		Vertex v1, v2;
		v1.color = color;
		v2.color = color;

		auto addLine = [&](int p1, int p2) {
			v1.position = positions[p1];
			v2.position = positions[p2];
			lineBuffer.push_back(v1);
			lineBuffer.push_back(v2);
		};

		addLine(0, 1);
		addLine(1, 2);
		addLine(2, 3);
		addLine(3, 0);
		addLine(4, 5);
		addLine(5, 6);
		addLine(6, 7);
		addLine(7, 4);
		addLine(0, 4);
		addLine(1, 5);
		addLine(2, 6);
		addLine(3, 7);
	}

	void GizmoRender::DrawCamera(const DirectX::XMMATRIX& transform, const std::array<DirectX::XMVECTOR, 8> frustrumPoints, DirectX::XMFLOAT3 color)
	{
		Vertex verticies[9];
		
		// origin
		XMStoreFloat3(&verticies[0].position, XMVector3Transform({ 0.0f, 0.0f, 0.0f, 1.0f }, transform));
		verticies[0].color = color;

		int i = 1;
		for (auto p : frustrumPoints)
		{
			XMStoreFloat3(&verticies[i].position, XMVector3Transform(p, transform));
			verticies[i].color = color;
			++i;
		}

		auto addLine = [&](int p1, int p2) {
			lineBuffer.push_back(verticies[p1]);
			lineBuffer.push_back(verticies[p2]);
		};

		// origin to near
		addLine(0, 1);
		addLine(0, 2);
		addLine(0, 3);
		addLine(0, 4);

		// near
		addLine(1, 2);
		addLine(2, 3);
		addLine(3, 4);
		addLine(4, 1);

		// near to far
		addLine(1, 5);
		addLine(2, 6);
		addLine(3, 7);
		addLine(4, 8);

		// far
		addLine(5, 6);
		addLine(6, 7);
		addLine(7, 8);
		addLine(8, 5);
	}

	void GizmoRender::CreateBuffers()
	{
		lineBuffer.reserve(maxGizmos * 10); // picked a number
		lineVBuffer.SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST); // first update called will initialise it

		// pre calc position buffer for the sphere Gizmo
		{
			spherePositions.reserve(pointsPerCircle * 3);

			float dAngle = XM_2PI / (float)pointsPerCircle;
			float angle;

			// around x
			angle = 0.0f;
			for (auto i = pointsPerCircle; i > 0; --i)
			{
				spherePositions.push_back({ 0.0f, sinf(angle), cosf(angle), 1.0f });
				angle += dAngle;
			}

			// around y
			angle = 0.0f;
			for (auto i = pointsPerCircle; i > 0; --i)
			{
				spherePositions.push_back({ cosf(angle), 0.0f, sinf(angle), 1.0f });
				angle += dAngle;
			}
			
			// around z
			angle = 0.0f;
			for (auto i = pointsPerCircle; i > 0; --i)
			{
				spherePositions.push_back({ cosf(angle), sinf(angle), 0.0f, 1.0f });
				angle += dAngle;
			}

		}
	}

	void GizmoRender::Draw()
	{
		if (lineBuffer.size() > 0)
		{
			lineVBuffer.Update(lineBuffer);
			lineVBuffer.Bind();
			lineVBuffer.Draw();
		}
	}

}
