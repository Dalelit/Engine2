#include "Boids2D.hlsli"

Texture2D<float4> textureIn : register(t0);
RWTexture2D<float4> textureOut : register(u0);

RWStructuredBuffer<Boid> boidBuffer : register(u1);

float2 Force(uint indx, Boid boid)
{
	float count = 1.0;
	float2 groupCentre = boid.position;
	float2 groupDirection = boid.direction;
	float2 groupRepulsion = float2(0.0, 0.0);

	for (int i = 0; i < boidCount; i++)
	{
		if ((uint)i != indx)
		{
			Boid other = boidBuffer[i];
			float dist = length(other.position - boid.position);

			if (dist <= boidSenseRadius)
			{
				count++;
				groupCentre += other.position;
				groupDirection += other.direction;

				// add the inverse sqr of the distance
				float2 toBoid = boid.position - other.position;
				float toBoidDistInv = boidSenseRadius - length(toBoid);
				groupRepulsion += normalize(toBoid) * toBoidDistInv * toBoidDistInv;
			}
		}
	}

	groupCentre /= count;
	groupCentre = groupCentre - boid.position;

	groupDirection /= count;
	groupDirection = groupDirection - boid.direction;

	if (count > 1.0) groupRepulsion /= (count - 1.0);

	return groupCentre * centreStrength + groupDirection * directionStrength + groupRepulsion * repulsionStrength;
}

[numthreads(1024, 1, 1)]
void BoidsUpdate(uint3 DTid : SV_DispatchThreadID)
{
	uint indx = DTid.x;

	Boid b = boidBuffer[indx];

	// update direciton
	b.direction = normalize(b.direction + Force(indx, b) * deltaTime);

	// move forward
	b.position += b.direction * boidSpeed * deltaTime;

	// handle world bounds... simply
	if (b.position.x < 0.0) b.position.x += worldDimension.x;
	else if (b.position.x > worldDimension.x) b.position.x -= worldDimension.x;

	if (b.position.y < 0.0) b.position.y += worldDimension.y;
	else if (b.position.y > worldDimension.y) b.position.y -= worldDimension.y;

	// update the trails texture
	uint2 bufferCoord = WorldToBufferSpace(b.position);
	textureOut[bufferCoord] = float4(b.color, 1.0);

	// update the boid
	boidBuffer[indx] = b;
}

float4 SimpleDiffuseSample(int x, int y)
{
	float4 pixel = float4(0.0, 0.0, 0.0, 0.0);

	// move the neighbouring values in
	pixel += textureIn[int2(x - 1, y)];
	pixel += textureIn[int2(x - 1, y - 1)];
	pixel += textureIn[int2(x, y - 1)];
	pixel += textureIn[int2(x + 1, y - 1)];
	pixel += textureIn[int2(x + 1, y)];
	pixel += textureIn[int2(x + 1, y + 1)];
	pixel += textureIn[int2(x, y + 1)];
	pixel += textureIn[int2(x - 1, y + 1)];
	pixel *= diffuseRate / 8.0; // sum of neighbours * amount diffused into this spot

	// add what remains of the current value
	pixel += textureIn[int2(x, y)] * (1.0 - diffuseRate);

	// fade the total amount
	pixel *= (1.0 - diffuseFade);

	pixel.a = 1.0;

	return pixel;
}

float4 AreaDiffuseSample(int x, int y)
{
	float4 pixel = float4(0.0, 0.0, 0.0, 1.0);

	int startx = max(0, x - diffuseRadius);
	int starty = max(0, y - diffuseRadius);
	int endx = min(screenDimension.x, x + diffuseRadius);
	int endy = min(screenDimension.y, y + diffuseRadius);
	float area = (float)((endx - startx + 1) * (endy - starty + 1));

	int2 indx = int2(startx, starty);
	while (indx.y <= endy)
	{
		while (indx.x <= endx)
		{
			pixel += textureIn[indx]; // add all the values from the area
			indx.x++;
		}

		indx.x = startx;
		indx.y++;
	}

	float4 currentPixel = textureIn[int2(x, y)];

	// average it out, exluding the current pixel
	pixel -= currentPixel;
	pixel *= diffuseRate / 8.0; // only the amount diffused

	pixel += currentPixel * (1.0 - diffuseRate); // keeping what isn't diffused for the current pixel

	pixel *= 1.0 - diffuseFade; // remove overall loss

	pixel.a = 1.0; // needed?

	return saturate(pixel);
}

[numthreads(20, 20, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	int x = DTid.x;
	int y = DTid.y;

	float4 pixel;

	pixel = SimpleDiffuseSample(x, y);
	//pixel = AreaDiffuseSample(x, y);

	textureOut[int2(x, y)] = pixel;
}
