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

	void GizmoRender::DrawAxis(const DirectX::XMMATRIX& instance)
	{
		Vertex v0, v1;
		auto tr = XMMatrixTranspose(instance); // engine stores the transform ready for the GPU... so undo that.

		XMStoreFloat3(&v0.position, XMVector3Transform({ 0.0f, 0.0f, 0.0f, 1.0f }, tr));

		// x axis
		v0.color = xAxisColor;
		v1.color = xAxisColor;
		XMStoreFloat3(&v1.position, XMVector3Transform({ 1.0f, 0.0f, 0.0f, 1.0f }, tr));
		lineBuffer.push_back(v0);
		lineBuffer.push_back(v1);

		// y axis
		v0.color = yAxisColor;
		v1.color = yAxisColor;
		XMStoreFloat3(&v1.position, XMVector3Transform({ 0.0f, 1.0f, 0.0f, 1.0f }, tr));
		lineBuffer.push_back(v0);
		lineBuffer.push_back(v1);

		// z axis
		v0.color = zAxisColor;
		v1.color = zAxisColor;
		XMStoreFloat3(&v1.position, XMVector3Transform({ 0.0f, 0.0f, 1.0f, 1.0f }, tr));
		lineBuffer.push_back(v0);
		lineBuffer.push_back(v1);
	}

	void GizmoRender::DrawSphere(const DirectX::XMMATRIX& instance)
	{
		auto tr = XMMatrixTranspose(instance); // engine stores the transform ready for the GPU... so undo that.
		auto position = spherePositions.begin();
		Vertex vStart, vCurrent, vPrevious;

		auto drawCircle = [&](XMFLOAT3 color) {
			vStart.color = color;
			vCurrent.color = color;
			XMStoreFloat3(&vStart.position, XMVector3Transform(*position, tr));
			vPrevious = vStart;
			++position;

			for (UINT i = 1; i < pointsPerCircle; ++i)
			{
				XMStoreFloat3(&vCurrent.position, XMVector3Transform(*position, tr));
				lineBuffer.push_back(vPrevious);
				lineBuffer.push_back(vCurrent);
				vPrevious = vCurrent;
				++position;
			}
			lineBuffer.push_back(vCurrent);
			lineBuffer.push_back(vStart);
		};

		drawCircle(xAxisColor);
		drawCircle(yAxisColor);
		drawCircle(zAxisColor);
	}

	void GizmoRender::DrawCube(const DirectX::XMMATRIX& instance)
	{
		XMFLOAT3 positions[8];
		auto tr = XMMatrixTranspose(instance); // engine stores the transform ready for the GPU... so undo that.

		XMFLOAT3* pos = positions;
		XMStoreFloat3(pos, XMVector3Transform({ -0.5f,  -0.5f,  -0.5f, 1.0f }, tr)); ++pos;
		XMStoreFloat3(pos, XMVector3Transform({ -0.5f,   0.5f,  -0.5f, 1.0f }, tr)); ++pos;
		XMStoreFloat3(pos, XMVector3Transform({  0.5f,   0.5f,  -0.5f, 1.0f }, tr)); ++pos;
		XMStoreFloat3(pos, XMVector3Transform({  0.5f,  -0.5f,  -0.5f, 1.0f }, tr)); ++pos;
		XMStoreFloat3(pos, XMVector3Transform({ -0.5f,  -0.5f,   0.5f, 1.0f }, tr)); ++pos;
		XMStoreFloat3(pos, XMVector3Transform({ -0.5f,   0.5f,   0.5f, 1.0f }, tr)); ++pos;
		XMStoreFloat3(pos, XMVector3Transform({  0.5f,   0.5f,   0.5f, 1.0f }, tr)); ++pos;
		XMStoreFloat3(pos, XMVector3Transform({  0.5f,  -0.5f,   0.5f, 1.0f }, tr));

		Vertex v1, v2;
		v1.color = cubeColor;
		v2.color = cubeColor;

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

	void GizmoRender::DrawCameraPerspective(const DirectX::XMMATRIX& instance, float nearPlane, float farPlane, float aspectRatio, float fieldOfView)
	{
		Vertex v;
		v.color = cameraColor;

		auto tr = XMMatrixTranspose(instance); // engine stores the transform ready for the GPU... so undo that.

		auto addLine = [&](XMFLOAT3 p1, XMFLOAT3 p2) {
			v.position = p1;
			lineBuffer.push_back(v);
			v.position = p2;
			lineBuffer.push_back(v);
		};

		float ydist = tanf(fieldOfView / 2.0f);
		float xdist = ydist * aspectRatio;

		XMFLOAT3 origin;
		XMStoreFloat3(&origin, XMVector3Transform({ 0.0f, 0.0f, 0.0f, 1.0f }, tr));

		XMFLOAT3 p0, p1, p2, p3;
		float pydist = ydist * nearPlane;
		float pxdist = xdist * nearPlane;
		XMStoreFloat3(&p0, XMVector3Transform({ -pxdist,  pydist, nearPlane, 1.0f }, tr));
		XMStoreFloat3(&p1, XMVector3Transform({  pxdist,  pydist, nearPlane, 1.0f }, tr));
		XMStoreFloat3(&p2, XMVector3Transform({  pxdist, -pydist, nearPlane, 1.0f }, tr));
		XMStoreFloat3(&p3, XMVector3Transform({ -pxdist, -pydist, nearPlane, 1.0f }, tr));

		addLine(p0, p1);
		addLine(p1, p2);
		addLine(p2, p3);
		addLine(p3, p0);

		pydist = ydist * farPlane;
		pxdist = xdist * farPlane;
		XMStoreFloat3(&p0, XMVector3Transform({ -pxdist,  pydist, farPlane, 1.0f }, tr));
		XMStoreFloat3(&p1, XMVector3Transform({  pxdist,  pydist, farPlane, 1.0f }, tr));
		XMStoreFloat3(&p2, XMVector3Transform({  pxdist, -pydist, farPlane, 1.0f }, tr));
		XMStoreFloat3(&p3, XMVector3Transform({ -pxdist, -pydist, farPlane, 1.0f }, tr));

		addLine(p0, p1);
		addLine(p1, p2);
		addLine(p2, p3);
		addLine(p3, p0);

		addLine(origin, p0);
		addLine(origin, p1);
		addLine(origin, p2);
		addLine(origin, p3);
	}

	void GizmoRender::DrawCameraOrthographic(const DirectX::XMMATRIX& instance, float nearPlane, float farPlane, float viewWidth, float viewHeight)
	{
		Vertex v;
		v.color = cameraColor;

		auto tr = XMMatrixTranspose(instance); // engine stores the transform ready for the GPU... so undo that.

		Vertex verticies[9];
		for (int i = 0; i < 9; ++i) verticies[i].color = cameraColor;

		auto addLine = [&](int p1, int p2) {
			lineBuffer.push_back(verticies[p1]);
			lineBuffer.push_back(verticies[p2]);
		};

		float xDist = viewWidth * 0.5f;
		float yDist = viewHeight * 0.5f;

		// origin
		XMStoreFloat3(&verticies[0].position, XMVector3Transform({ 0.0f, 0.0f, 0.0f, 1.0f }, tr));

		// near plane
		XMStoreFloat3(&verticies[1].position, XMVector3Transform({ -xDist,  yDist, nearPlane, 1.0f }, tr));
		XMStoreFloat3(&verticies[2].position, XMVector3Transform({  xDist,  yDist, nearPlane, 1.0f }, tr));
		XMStoreFloat3(&verticies[3].position, XMVector3Transform({  xDist, -yDist, nearPlane, 1.0f }, tr));
		XMStoreFloat3(&verticies[4].position, XMVector3Transform({ -xDist, -yDist, nearPlane, 1.0f }, tr));

		// far plane
		XMStoreFloat3(&verticies[5].position, XMVector3Transform({ -xDist,  yDist, farPlane, 1.0f }, tr));
		XMStoreFloat3(&verticies[6].position, XMVector3Transform({  xDist,  yDist, farPlane, 1.0f }, tr));
		XMStoreFloat3(&verticies[7].position, XMVector3Transform({  xDist, -yDist, farPlane, 1.0f }, tr));
		XMStoreFloat3(&verticies[8].position, XMVector3Transform({ -xDist, -yDist, farPlane, 1.0f }, tr));

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
