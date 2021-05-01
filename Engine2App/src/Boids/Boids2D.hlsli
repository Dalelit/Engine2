static const float PI = 3.141592654;
static const float TwoPI = 6.283185307;

struct Boid {
	float3 position;
	float  rotation;
	float  scale;
	float3 color;
};

cbuffer WorldInfo : register(b1)
{
	float2 worldDimension;
	uint2  screenDimension;
};

cbuffer ControlInfo : register(b3)
{
	float time;
	float deltaTime;
	int   diffuseRadius;
	float diffuseRate;
	float diffuseFade;
	int   xMouse;
	int   yMouse;
	int   boidCount;
	float boidSpeed;
	float boidScale;
};

float2 RotateVector(float2 vec, float radians)
{
	return float2(vec.x * cos(radians) - vec.y * sin(radians), vec.x * sin(radians) + vec.y * cos(radians));
}

float2 UnitVector(float angle)
{
	return float2(cos(angle), sin(angle));
}

float2 WorldToScreenSpace(float2 world)
{
	float x = world.x / worldDimension.x * 2.0 - 1.0;
	float y = world.y / worldDimension.y * 2.0 - 1.0;
	return float2(x, y);
}

uint2 WorldToBufferSpace(float2 world)
{
	uint x = world.x / worldDimension.x * screenDimension.x;
	uint y = (1.0 - world.y / worldDimension.y) * screenDimension.y;
	return uint2(x, y);
}



