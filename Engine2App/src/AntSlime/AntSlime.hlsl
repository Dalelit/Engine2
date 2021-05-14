Texture2D<float4> buffer1in : register(t0);
RWTexture2D<float4> buffer2out : register(u0);

static const float PI = 3.141592654;
static const float TWOPI = 6.283185307;

struct Ant
{
	int    type;
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
	int   senseRadius;
	float replusion;
	float speed;
	float steeringStrength;
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

	pixel.a = 1.0;

	buffer2out[int2(x, y)] = pixel;
}

float4 GetLocationInfo(float2 location)
{
	int startx = max(0, location.x - senseRadius);
	int starty = max(0, location.y - senseRadius);

	int endx = min(xmax - 1, location.x + senseRadius);
	int endy = min(ymax - 1, location.y + senseRadius);

	int2 indx = int2(startx, starty);
	float4 value = 0.0;

	while (indx.y <= endy)
	{
		while (indx.x <= endx)
		{
			value += buffer1in[indx];
			indx.x++;
		}

		indx.x = startx;
		indx.y++;
	}

	return value;
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

	float4 frontInfo = GetLocationInfo(ant.position + frontDir * senseRange);
	float4 leftInfo = GetLocationInfo(ant.position + leftDir * senseRange);
	float4 rightInfo = GetLocationInfo(ant.position + rightDir * senseRange);

	float front = 0;
	float left = 0;
	float right = 0;

	if (ant.type == 0)
	{
		front = frontInfo.x - (frontInfo.y + frontInfo.z) * replusion;
		left  = leftInfo.x  - (leftInfo.y  + frontInfo.z) * replusion;
		right = rightInfo.x - (rightInfo.y + frontInfo.z) * replusion;
	}
	else if (ant.type == 1)
	{
		front = frontInfo.y - (frontInfo.x + frontInfo.z) * replusion;
		left  = leftInfo.y  - (leftInfo.x  + frontInfo.z) * replusion;
		right = rightInfo.y - (rightInfo.x + frontInfo.z) * replusion;
	}
	else // type == 2
	{
		front = frontInfo.z - (frontInfo.x + frontInfo.y) * replusion;
		left =  leftInfo.z  - (leftInfo.x  + frontInfo.y) * replusion;
		right = rightInfo.z - (rightInfo.x + frontInfo.y) * replusion;
	}

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

	float4 val = buffer2out[int2(a.position)];
	if (a.type == 0) val.x = 1.0;
	else if (a.type == 1) val.y = 1.0;
	else val.z = 1.0;
	buffer2out[int2(a.position)] = val;
}