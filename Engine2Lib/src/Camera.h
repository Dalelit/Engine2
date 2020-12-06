#pragma once
#include "pch.h"
#include "Events.h"

namespace Engine2
{
	class Ray
	{
	public:
		DirectX::XMVECTOR origin;
		DirectX::XMVECTOR direction;
	};

	class Camera
	{
	public:
		Camera() {}
		Camera(const std::string& name) : name(name) {}
		virtual ~Camera() = default;

		void Update(DirectX::XMVECTOR position, DirectX::XMVECTOR rotation);

		void LoadViewProjectionMatrixT(DirectX::XMMATRIX& vpMatrix) { vpMatrix = DirectX::XMMatrixTranspose(viewProjectionMatrix); }; // transpose for DX.

		virtual void OnImgui();

		inline float AspectRatio() const { return aspectRatio; }
		inline float NearPlane() const { return nearZ; }
		inline float FarPlane() const { return farZ; }
		inline float FieldOfView() const { return fov; }

		inline void SetAspectRatio(float ratio) { aspectRatio = ratio; }
		inline bool IsAspectRatioLockedToScreen() { return aspectRatioLockedToScreen; }
		inline bool SetAspectRatioLockedToScreen(bool locked = true) { aspectRatioLockedToScreen = locked; }

		inline std::string& GetName() { return name; }
		inline void SetName(const std::string& newName) { name = newName; }

		// Origin is the relative point on the screen (0,0) to (1,1). NDC have top left as origin.
		//Ray ScreenCoordToRay(float x, float y);

	protected:
		std::string name;
		float aspectRatio = 1.0f;
		float fov = DirectX::XMConvertToRadians(75.0f);
		float nearZ = 0.5f;
		float farZ = 1000.0f;
		bool aspectRatioLockedToScreen = true;

		static constexpr DirectX::XMVECTOR FORWARD = { 0.0f, 0.0f, 1.0f, 1.0f };
		static constexpr DirectX::XMVECTOR UP = { 0.0f, 1.0f, 0.0f, 1.0f };

		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projectionMatrix;
		DirectX::XMMATRIX viewProjectionMatrix;
	};
}
