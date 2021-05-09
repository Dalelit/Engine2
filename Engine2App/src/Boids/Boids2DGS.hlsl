#include "Boids2D.hlsli"

StructuredBuffer<Boid> boidBuffer : register(t1);

struct GSOutput
{
	float4 pos : SV_POSITION;
};

[maxvertexcount(100)]
void main(
	point VSOutSenseLines input[1] : SV_POSITION,
	inout LineStream< GSOutput > output
)
{
	GSOutput startPoint;
	GSOutput endPoint;

	startPoint.pos = float4(WorldToScreenSpace(input[0].posWS.xy), 0.0, 1.0);

	uint instId = input[0].index;
	Boid b = boidBuffer[instId];

	for (uint i = instId + 1; i < (uint)boidCount; i++)
	{
		Boid other = boidBuffer[i];
		float dist = length(input[0].posWS.xy - other.position);

		if (dist <= boidSenseRadius)
		{
			output.Append(startPoint);
			endPoint.pos = float4(WorldToScreenSpace(other.position), 0.0, 1.0);
			output.Append(endPoint);
			output.RestartStrip();
		}
	}
}