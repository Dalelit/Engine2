Texture2D<float4> buffer1in : register(t0);
RWTexture2D<float4> buffer2out : register(u0);

static const float PI = 3.141592654;
static const float TWOPI = 6.283185307;
static const float ONEQTRPI = PI * 0.25;
static const float HALFPI = PI * 0.5;
static const float THREEHALFPI = PI * 1.5;

struct Ant
{
	float2 position;
	float  angle;
};

RWStructuredBuffer<Ant> bufferAnts : register(u1);

cbuffer ControlInfo : register(b3)
{
	float xmax;
	float ymax;
	float locationRange;
	float diffuseLoss;
	float diffuseFade;
	float senseAngle;
	float senseRange;
	float speed;
	float steeringStrength;
	float boundaryForce;
	float boundaryRange;
	float time;
	float deltaTime;
};

float Random(float2 p)
{
	return frac(sin(time * p.x * p.y) * 12345.211);
}

[numthreads(20, 20, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	int x = DTid.x;
	int y = DTid.y;

	float retain = 1.0 - diffuseLoss;
	float gain = diffuseLoss / 8.0;

	float4 pixel = float4(0.0, 0.0, 0.0, 0.0);

	// move the neighbouring values in
	pixel += buffer1in[int2(x - 1, y)];
	pixel += buffer1in[int2(x - 1, y - 1)];
	pixel += buffer1in[int2(x, y - 1)];
	pixel += buffer1in[int2(x + 1, y - 1)];
	pixel += buffer1in[int2(x + 1, y)];
	pixel += buffer1in[int2(x + 1, y + 1)];
	pixel += buffer1in[int2(x, y + 1)];
	pixel += buffer1in[int2(x - 1, y + 1)];
	pixel *= gain; // sum of neighbours * amount diffused into this spot

	// add what remains of the current value
	pixel += buffer1in[int2(x, y)] * retain;

	// fade the total amount
	pixel *= diffuseFade;

	pixel.z = 0.0;
	pixel.a = 1.0;

	buffer2out[int2(x, y)] = pixel;
}

float4 GetLocationInfo(float2 location)
{
	uint2 indx = uint2(location);

	if (indx.x < 0) indx.x += xmax;
	else if (indx.x >= (uint)xmax) indx.x -= xmax;

	if (indx.y < 0) indx.y += ymax;
	else if (indx.y >= (uint)ymax) indx.y -= ymax;

	return buffer1in[indx];
}

float2 RotateVector(float2 vec, float radians)
{
	float2x2 mat = float2x2(cos(radians), -sin(radians), sin(radians), cos(radians));
	return mul(vec, mat);
}

float2 Direction(float angle)
{
	return float2(cos(angle), sin(angle));
}

float GetAntDirectionChangeToCentre(Ant ant)
{
	return atan2(ymax * 0.5 - ant.position.y, xmax * 0.5 - ant.position.x) - ant.angle;
}

float GetAntDirectionToCentre(Ant ant)
{
	return atan2(ymax * 0.5 - ant.position.y, xmax * 0.5 - ant.position.x);
}

float GetAntDirectionChange(Ant ant)
{
	// check to follow
	float2 frontDir = Direction(ant.angle);
	float2 leftDir = Direction(ant.angle + senseAngle);
	float2 rightDir = Direction(ant.angle - senseAngle);

	float front = GetLocationInfo(ant.position + frontDir * senseRange).x;
	float left = GetLocationInfo(ant.position + leftDir * senseRange).x;
	float right = GetLocationInfo(ant.position + rightDir * senseRange).x;

	if (front > max(left, right)) return 0.0;
	else if (front < min(left, right)) return (Random(ant.position) - 0.5) * 2.0 * steeringStrength * deltaTime;
	else if (left > right) return senseAngle * steeringStrength * deltaTime;
	else return -senseAngle * steeringStrength * deltaTime;
}

void BoundaryCheck(inout Ant ant)
{
	float xdistance = min(ant.position.x, xmax - ant.position.x);
	float ydistance = min(ant.position.y, ymax - ant.position.y);
	float minDistance = min(xdistance, ydistance);

	if (minDistance < boundaryRange)
	{
		ant.angle = GetAntDirectionToCentre(ant) + ((Random(ant.position) - 0.5) * senseAngle);
	}
}

[numthreads(1024, 1, 1)]
void UpdateAnts(uint3 DTid : SV_DispatchThreadID)
{
	uint indx = DTid.x;

	Ant a = bufferAnts[indx];

	a.angle += GetAntDirectionChange(a);
	a.position += Direction(a.angle) * speed * deltaTime;

	BoundaryCheck(a);

	bufferAnts[indx] = a;

	// put it on the map.
	buffer2out[int2(a.position)] = float4(1.0, 1.0, 1.0, 1.0);
}