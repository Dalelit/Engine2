#include "SceneCBVS.hlsl"

cbuffer entityConst : register (b1)
{
	matrix entityTransform;
	matrix entityTransformRotation;
};
