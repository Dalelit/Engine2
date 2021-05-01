#include "Boids2D.hlsli"

StructuredBuffer<Boid> boidBuffer : register(t1);

struct VSOut
{
	float3 col   : Color;
	float2 uv    : UV;
	float4 posSS : SV_POSITION;
};

VSOut main(float3 pos : Position, float2 uv : UV, uint instId : SV_InstanceID)
{
	VSOut vso;

	Boid b = boidBuffer[instId];

	float2 posWS = RotateVector(pos.xy * b.scale, b.rotation) + b.position.xy;

	vso.posSS = float4(WorldToScreenSpace(posWS), 0.0, 1.0);
	vso.uv = uv;
	vso.col = b.color;

	return vso;
}
