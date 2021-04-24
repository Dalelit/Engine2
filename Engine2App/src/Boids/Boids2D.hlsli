static const float XM_PI = 3.141592654;
static const float XM_2PI = 6.283185307;

struct Boid {
	float3 position;
	float  rotation;
	float  scale;
	float3 color;
};

float2 RotateVector(float2 vec, float radians)
{
	float2x2 mat = float2x2(cos(radians), -sin(radians), sin(radians), cos(radians));
	return mul(vec, mat);
}

