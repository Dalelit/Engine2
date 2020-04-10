#include "pch.h"
#include "Gizmos.h"
#include "Common.h"

#define MAXVERTICIES 20000
#define E2_GIZMO_CHECK(num) E2_ASSERT(vertexCount + num < MAXVERTICIES, "Exceeded gizmos line buffer");

using namespace DirectX;

namespace Engine2
{

	GizmosBuffer::GizmosBuffer() : lineBuffer(MAXVERTICIES), vertexBuffer(MAXVERTICIES)
	{
		VertexShaderLayout vsLayout = { {"Position", DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT} };
		std::string vsFileName = Config::directories["ShaderCompiledDir"] + "GizmosVS.cso";
		pVS = VertexShader::CreateFromCompiledFile(vsFileName, vsLayout);

		std::string psFileName = Config::directories["ShaderCompiledDir"] + "GizmosPS.cso";
		pPS = PixelShader::CreateFromCompiledFile(psFileName);
	}

	void GizmosBuffer::NewFrame()
	{
		next = lineBuffer.begin();
		vertexCount = 0;
	}

	void GizmosBuffer::DrawLine(DirectX::XMVECTOR p0, DirectX::XMVECTOR p1)
	{
		E2_GIZMO_CHECK(2);

		*next++ = p0;
		*next++ = p1;
		vertexCount += 2;
	}

	void GizmosBuffer::DrawAxis(DirectX::XMFLOAT3 p0)
	{
		constexpr XMVECTOR right = { 1.0f, 0.0f, 0.0f, 0.0f };
		constexpr XMVECTOR up    = { 0.0f, 1.0f, 0.0f, 0.0f };
		constexpr XMVECTOR fwd   = { 0.0f, 0.0f, 1.0f, 0.0f };

		XMVECTOR v0 = XMLoadFloat3(&p0);
		v0.m128_f32[3] = 1.0f;

		E2_GIZMO_CHECK(6);
		*next++ = v0;
		*next++ = v0 + right;
		*next++ = v0;
		*next++ = v0 + up;
		*next++ = v0;
		*next++ = v0 + fwd;
		vertexCount += 6;
	}

	void GizmosBuffer::Draw()
	{
		if (!active) return;

		vertexBuffer.Update(lineBuffer.data(), vertexCount);

		// Note: Assuming the scene has already bound the camera matrix to VS constant buffer 0
		pVS->Bind();
		pPS->Bind();
		vertexBuffer.Bind();
		vertexBuffer.Draw();
	}

	void GizmosBuffer::OnImgui()
	{
		if (ImGui::TreeNode("Gizmos"))
		{
			ImGui::Checkbox("Active", &active);
			ImGui::Text("Vertex count %i", vertexCount);
			ImGui::TreePop();
		}
	}
}
