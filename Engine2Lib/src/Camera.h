#pragma once
#include "pch.h"
#include "Events.h"
#include "Serialiser.h"

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
		inline const DirectX::XMMATRIX& GetViewProjectionMatrix() const { return viewProjectionMatrix; }

		void SetOrthographic() { orthographic = true; }
		void SetPerspective() { orthographic = false; }
		bool IsOrthographic() { return orthographic; }
		bool IsPerspective() { return !orthographic; }

		virtual void OnImgui();
		void Serialise(Serialisation::INode& node);

		inline float AspectRatio() const { return aspectRatio; }
		inline float NearPlane() const { return nearZ; }
		inline float FarPlane() const { return farZ; }

		inline float FieldOfView() const { return fov; }
		inline void SetFieldOfView(float newfov) { fov = newfov; }

		inline float ViewWidth() const { return viewWidth; }
		inline float ViewHeight() const { return viewHeight; }
		inline void SetViewWidth(float width) { viewWidth = width; }
		inline void SetViewHeight(float height) { viewHeight = height; }

		inline void SetAspectRatio(float ratio) { aspectRatio = ratio; viewWidth = viewHeight * aspectRatio; }
		inline bool IsAspectRatioLockedToScreen() { return aspectRatioLockedToScreen; }
		inline bool SetAspectRatioLockedToScreen(bool locked = true) { aspectRatioLockedToScreen = locked; }

		inline void SetNearPlane(float distance) { nearZ = std::max<float>(distance, 0.0f); }
		inline void SetFarPlane(float distance) { farZ = std::max<float>(distance, nearZ); }

		// top-left, top-right, bottom-right, bottom-left for near then far
		std::array<DirectX::XMVECTOR, 8> GetFrustrumPoints() const;

		DirectX::XMVECTOR GetFrustrumCentre() const;

		inline std::string& GetName() { return name; }
		inline void SetName(const std::string& newName) { name = newName; }

		// Origin is the relative point on the screen (0,0) to (1,1). NDC have top left as origin.
		//Ray ScreenCoordToRay(float x, float y);

	protected:
		std::string name;
		bool orthographic = false;
		float aspectRatio = 1.0f;
		float fov = DirectX::XMConvertToRadians(60.0f);
		float nearZ = 0.5f;
		float farZ = 50.0f;
		bool aspectRatioLockedToScreen = true;
		float viewWidth = 20.0f; // updated when aspectRatio set
		float viewHeight = 20.0f;

		static constexpr DirectX::XMVECTOR FORWARD = { 0.0f, 0.0f, 1.0f, 1.0f };
		static constexpr DirectX::XMVECTOR UP = { 0.0f, 1.0f, 0.0f, 1.0f };

		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projectionMatrix;
		DirectX::XMMATRIX viewProjectionMatrix;

		std::array<DirectX::XMVECTOR, 8> GetFrustrumPointsOrthographic() const;
		std::array<DirectX::XMVECTOR, 8> GetFrustrumPointsPerspective() const;
	};

	// Helper class
	class WorldCamera
	{
	public:
		const Camera& camera;
		const DirectX::XMMATRIX& worldTransform;

		WorldCamera(const Camera& camera, const DirectX::XMMATRIX& worldTransformMatrix) : camera(camera), worldTransform(worldTransformMatrix) {}

		std::array<DirectX::XMVECTOR, 8> GetFrustrumPoints();

		DirectX::XMVECTOR GetFrustrumCentre();
	};
}
