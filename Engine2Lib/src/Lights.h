#pragma once
#include "pch.h"
#include "Common.h"
#include "Camera.h"
#include "Offscreen.h"

namespace Engine2
{
	class PointLight
	{
	public:
		PointLight() = default;
		PointLight(DirectX::XMVECTOR color) : color(color) {}

		void OnImgui();

		DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f, 1.0f};
	};

	class DirectionalLight
	{
	public:
		DirectionalLight();

		void OnImgui();

		inline void SetRotation(float x, float y, float z) { SetRotation({x, y, z, 1.0f}); }
		inline void SetRotation(DirectX::XMVECTOR rot) { rotation = rot; CalcPosition(); }

		inline Camera& GetCamera() { return camera; }

		DirectX::XMMATRIX GetModelViewProjectionMatrixT();

		DirectX::XMMATRIX& GetTransform() { return transform; }
		DirectX::XMMATRIX GetTransformT() { return DirectX::XMMatrixTranspose(GetTransform()); }

		bool ShowGizmos() { return showGizmos; }

	protected:
		DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMVECTOR rotation;
		DirectX::XMMATRIX transform;
		Camera camera;

		DirectX::XMVECTOR centre = DirectX::g_XMZero;
		DirectX::XMVECTOR position;

		bool showGizmos = true;

		void CalcPosition();
	};
}