cbuffer sceneConst : register (b0)
{
	float4 cameraPosition;
	uint   numberOfPointLights;
};

struct pointLightData
{
	float3 position;
	float3 color;
};

StructuredBuffer<pointLightData> pointLights : register(t2);
