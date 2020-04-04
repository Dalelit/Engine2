Texture2D tex;
SamplerState smplr;

float4 main(float2 texCoord : TexCoord) : SV_TARGET
{
	//return tex.Sample(smplr, texCoord);

	float4 sum = 0.0;
	int size = 9;
	int rad = size / 2;

	int width, height;
	tex.GetDimensions(width, height);
	float dX = 1.0f / width;
	float dY = 1.0f / height;

	float xStart = texCoord.x - ((float)rad * dX);
	float yStart = texCoord.y - ((float)rad * dY);
	float2 coord = float2(xStart, yStart);

	for (int y = -rad; y <= rad; y++)
	{
		coord.x = xStart;
		for (int x = -rad; x <= rad; x++)
		{
			sum += tex.Sample(smplr, coord);
			coord.x += dX;
		}
		coord.y += dY;
	}

	return sum / (float)(size * size);
}
