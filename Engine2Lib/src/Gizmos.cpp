#include "pch.h"
#include "Gizmos.h"
#include "Common.h"

#define MAXVERTICIES 20000
#define E2_GIZMO_CHECK(num) E2_ASSERT(vertexCount + num < MAXVERTICIES, "Exceeded gizmos line buffer");

using namespace DirectX;

namespace Engine2
{

	Gizmos::Gizmos() : lineBuffer(MAXVERTICIES), vertexBuffer(MAXVERTICIES)
	{
		VertexShaderLayout vsLayout = { {"Position", DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT} };
		std::string vsFileName = Config::directories["ShaderCompiledDir"] + "GizmosVS.cso";
		pVS = VertexShader::CreateFromCompiledFile(vsFileName, vsLayout);

		std::string psFileName = Config::directories["ShaderCompiledDir"] + "GizmosPS.cso";
		pPS = PixelShader::CreateFromCompiledFile(psFileName);
	}

	void Gizmos::NewFrame()
	{
		next = lineBuffer.begin();
		vertexCount = 0;
	}

	void Gizmos::DrawLine(DirectX::XMVECTOR p0, DirectX::XMVECTOR p1)
	{
		E2_GIZMO_CHECK(2);

		*next++ = p0;
		*next++ = p1;
		vertexCount += 2;
	}

	void Gizmos::AddLines(DirectX::XMVECTOR pos, std::vector<DirectX::XMVECTOR>& points)
	{
		E2_GIZMO_CHECK(points.size());

		for (auto& p : points) *next++ = pos + p;
		vertexCount += (unsigned int)points.size();
	}

	void Gizmos::Draw()
	{
		if (!active) return;

		vertexBuffer.Update(lineBuffer.data(), vertexCount);

		// Note: Assuming the scene has already bound the camera matrix to VS constant buffer 0
		pVS->Bind();
		pPS->Bind();
		vertexBuffer.Bind();
		vertexBuffer.Draw();
	}

	void Gizmos::OnImgui()
	{
		if (ImGui::TreeNode("Gizmos"))
		{
			ImGui::Checkbox("Active", &active);
			ImGui::Text("Vertex count %i", vertexCount);
			ImGui::TreePop();
		}
	}

	std::vector<XMVECTOR> Gizmos::AxisWireframe = {
		{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f},
	};

	std::vector<XMVECTOR> Gizmos::SphereWireframe = {
		{0.500f, 0.0f, 0.000f, 1.0f},{0.433f, 0.0f, 0.250f, 1.0f},
		{0.433f, 0.0f, 0.250f, 1.0f},{0.250f, 0.0f, 0.433f, 1.0f},
		{0.250f, 0.0f, 0.433f, 1.0f},{0.000f, 0.0f, 0.500f, 1.0f},
		{0.000f, 0.0f, 0.500f, 1.0f},{-0.250f, 0.0f, 0.433f, 1.0f},
		{-0.250f, 0.0f, 0.433f, 1.0f},{-0.433f, 0.0f, 0.250f, 1.0f},
		{-0.433f, 0.0f, 0.250f, 1.0f},{-0.500f, 0.0f, 0.000f, 1.0f},
		{-0.500f, 0.0f, 0.000f, 1.0f},{-0.433f, 0.0f, -0.250f, 1.0f},
		{-0.433f, 0.0f, -0.250f, 1.0f},{-0.250f, 0.0f, -0.433f, 1.0f},
		{-0.250f, 0.0f, -0.433f, 1.0f},{0.000f, 0.0f, -0.500f, 1.0f},
		{0.000f, 0.0f, -0.500f, 1.0f},{0.250f, 0.0f, -0.433f, 1.0f},
		{0.250f, 0.0f, -0.433f, 1.0f},{0.433f, 0.0f, -0.250f, 1.0f},
		{0.433f, 0.0f, -0.250f, 1.0f},{0.500f, 0.0f, 0.000f, 1.0f},
		{0.0f, 0.000f, 0.500f, 1.0f},{0.0f, 0.250f, 0.433f, 1.0f},
		{0.0f, 0.250f, 0.433f, 1.0f},{0.0f, 0.433f, 0.250f, 1.0f},
		{0.0f, 0.433f, 0.250f, 1.0f},{0.0f, 0.500f, 0.000f, 1.0f},
		{0.0f, 0.500f, 0.000f, 1.0f},{0.0f, 0.433f, -0.250f, 1.0f},
		{0.0f, 0.433f, -0.250f, 1.0f},{0.0f, 0.250f, -0.433f, 1.0f},
		{0.0f, 0.250f, -0.433f, 1.0f},{0.0f, 0.000f, -0.500f, 1.0f},
		{0.0f, 0.000f, -0.500f, 1.0f},{0.0f, -0.250f, -0.433f, 1.0f},
		{0.0f, -0.250f, -0.433f, 1.0f},{0.0f, -0.433f, -0.250f, 1.0f},
		{0.0f, -0.433f, -0.250f, 1.0f},{0.0f, -0.500f, 0.000f, 1.0f},
		{0.0f, -0.500f, 0.000f, 1.0f},{0.0f, -0.433f, 0.250f, 1.0f},
		{0.0f, -0.433f, 0.250f, 1.0f},{0.0f, -0.250f, 0.433f, 1.0f},
		{0.0f, -0.250f, 0.433f, 1.0f},{0.0f, 0.000f, 0.500f, 1.0f},
	};
}
