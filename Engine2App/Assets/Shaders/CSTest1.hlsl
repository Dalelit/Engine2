Texture2D<float4> buffer1in : register(t0);
RWTexture2D<float4> buffer2out : register(u0);

float4 readPixel(int x, int y)
{
	int2 indx = int2(x, y);
	return buffer1in[indx];
}

void writePixel(int x, int y, float4 value)
{
	int2 indx = int2(x, y);
	buffer2out[indx] = value;
}

[numthreads(20, 10, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	float4 pixel = readPixel(DTid.x, DTid.y);

	if (pixel.x == 1.0) pixel.x = 0.0; else pixel.x = 1.0;
	if (pixel.y == 1.0) pixel.y = 0.0; else pixel.y = 1.0;
	
	pixel.z += 0.01;
	if (pixel.z > 1.0) pixel.z = 0.0;

	writePixel(DTid.x, DTid.y, pixel);
}