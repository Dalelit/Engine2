#pragma once
#include "pch.h"

namespace Engine2
{
	namespace Math
	{
		using namespace DirectX;

		inline DirectX::XMMATRIX TranslationMatrix(DirectX::XMFLOAT3 position) { return DirectX::XMMatrixTranslation(position.x, position.y, position.z); }
		inline DirectX::XMMATRIX TranslationMatrix(DirectX::XMVECTOR position) { return DirectX::XMMatrixTranslationFromVector(position); }

		// More efficient way to create a transformation matrix.
		
		//Assume translation.w = 1.0.
		inline XMMATRIX TransformMatrixEuler(DirectX::XMVECTOR translation, DirectX::XMVECTOR scale, DirectX::XMVECTOR eulerRotation)
		{
			// Same as XMMatrixScalingFromVector(scale) * XMMatrixRotationRollPitchYawFromVector(rotation) * XMMatrixTranslationFromVector(translation)

			XMMATRIX transform = XMMatrixRotationRollPitchYawFromVector(eulerRotation);
			transform.r[0] *= scale.m128_f32[0];
			transform.r[1] *= scale.m128_f32[1];
			transform.r[2] *= scale.m128_f32[2];
			transform.r[3] = translation; //  = XMVectorSetW(pParticle->position, 1.0f); // if wanting to force w = 1.0
			return transform;
		}

		// Assume translation.w = 1.0.
		inline XMMATRIX TransformMatrixEuler(DirectX::XMVECTOR translation, DirectX::XMVECTOR eulerRotation)
		{
			XMMATRIX transform = XMMatrixRotationRollPitchYawFromVector(eulerRotation);
			transform.r[3] = translation; //  = XMVectorSetW(pParticle->position, 1.0f); // if wanting to force w = 1.0
			return transform;
		}

		// Assume translation.w = 1.0.
		inline XMMATRIX TransformMatrixQuat(DirectX::XMVECTOR translation, DirectX::XMVECTOR scale, DirectX::XMVECTOR quaternion)
		{
			// Same as XMMatrixScalingFromVector(scale) * XMMatrixRotationQuaternion(quaternion) * XMMatrixTranslationFromVector(translation)

			XMMATRIX transform = XMMatrixRotationQuaternion(quaternion);
			transform.r[0] *= scale.m128_f32[0];
			transform.r[1] *= scale.m128_f32[1];
			transform.r[2] *= scale.m128_f32[2];
			transform.r[3] = translation; //  = XMVectorSetW(pParticle->position, 1.0f); // if wanting to force w = 1.0
			return transform;
		}

		// https://stackoverflow.com/questions/1996957/conversion-euler-to-matrix-and-matrix-to-euler
		// Assume pure rotation matrix
		inline XMVECTOR RotationMatrixDecomposeYawPitchRoll(XMMATRIX mat)
		{
			XMVECTOR euler = { 0.0f , 0.0f , 0.0f , 0.0f };

			euler.m128_f32[0] = asinf(-mat.r[2].m128_f32[1]);      // Pitch mat._32
			if (cosf(euler.m128_f32[0]) > 0.00001f)                // Not at poles
			{
				euler.m128_f32[1] = atan2f(mat.r[2].m128_f32[0], mat.r[2].m128_f32[2]);     // Yaw mat._31, mat._33
				euler.m128_f32[2] = atan2f(mat.r[0].m128_f32[1], mat.r[1].m128_f32[1]);     // Roll mat._12, mat._22
			}
			else
			{
				euler.m128_f32[1] = 0.0f;                         // Yaw
				euler.m128_f32[2] = atan2f(-mat.r[1].m128_f32[0], mat.r[0].m128_f32[0]);    // Roll -mat._21, mat._11
			}

			return euler;
		}

		inline XMVECTOR QuaternionToEuler(XMVECTOR q)
		{
			return RotationMatrixDecomposeYawPitchRoll(XMMatrixRotationQuaternion(q));
		}

		inline XMVECTOR DegToRad(XMVECTOR v) { return v * (XM_PI / 180.0f); }
		inline XMVECTOR RadToDeg(XMVECTOR v) { return v * (180.0f / XM_PI); }
		
		inline float DegToRad(float f) { return f * (XM_PI / 180.0f); }
		inline float RadToDeg(float f) { return f * (180.0f / XM_PI); }

	}
}