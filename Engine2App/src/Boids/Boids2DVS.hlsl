#include "Boids2D.hlsli"

StructuredBuffer<Boid> boidBuffer : register(t1);

struct VSOut
{
	float3 col   : Color;
	float2 uv    : UV;
	float4 posSS : SV_POSITION;
};

VSOut main(float2 pos : Position, float2 uv : UV, uint instId : SV_InstanceID)
{
	VSOut vso;

	Boid b = boidBuffer[instId];

	float2 posWS = RotateVector(pos * boidScale, b.direction) + b.position;

	vso.posSS = float4(WorldToScreenSpace(posWS), 0.0, 1.0);
	vso.uv = uv;
	vso.col = b.color;

	return vso;
}

struct VSSenseOut
{
	float2 uv    : UV;
	float4 posSS : SV_POSITION;
};

VSSenseOut sense(float2 pos : Position, float2 uv : UV, uint instId : SV_InstanceID)
{
	VSSenseOut vso;

	Boid b = boidBuffer[instId];

	float2 posWS = RotateVector(pos * boidSenseRadius, b.direction) + b.position;

	vso.posSS = float4(WorldToScreenSpace(posWS), 0.0, 1.0);
	vso.uv = uv;

	return vso;
}

VSOutSenseLines senseLines(float2 pos : Position, uint instId : SV_InstanceID)
{
	Boid b = boidBuffer[instId];
	VSOutSenseLines vsout;
	vsout.index = instId;
	vsout.posWS = float4(b.position, 0.0, 1.0);
	return vsout;
}
