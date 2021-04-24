#include "Boids2D.hlsli"

Texture2D<float4> textureIn : register(t0);
RWTexture2D<float4> textureOut : register(u0);

cbuffer ControlInfo : register(b3)
{
	float time;
	float deltaTime;
	int   xmax;
	int   ymax;
	int   diffuseRadius;
	float diffuseRate;
	float diffuseFade;
	int   xMouse;
	int   yMouse;
	int   boidCount;
};


RWStructuredBuffer<Boid> boidBuffer : register(u1);

[numthreads(1024, 1, 1)]
void BoidsUpdate(uint3 DTid : SV_DispatchThreadID)
{
	uint indx = DTid.x;

	Boid b = boidBuffer[indx];

	b.rotation += deltaTime * 1.5;
	b.color.b = (float)indx / (float)boidCount;
	if (b.color.b > 1.0) b.color.b = 0.0;

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
	int endx = min(xmax, x + diffuseRadius);
	int endy = min(ymax, y + diffuseRadius);
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

	if (abs(xMouse - x) < 5 && abs(yMouse - y) < 5)
	{
		pixel = float4(1.0, 0.0, 0.0, 1.0);
	}
	else
	{
		pixel = SimpleDiffuseSample(x, y);
		//pixel = AreaDiffuseSample(x, y);
	}

	textureOut[int2(x, y)] = pixel;
}
