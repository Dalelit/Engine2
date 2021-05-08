float4 main(float3 col : Color, float2 uv : UV) : SV_TARGET
{
	return float4(col, 1.0);;
}

float4 sense(float2 uv : UV) : SV_TARGET
{
	float4 pixel = 0.0;

	uv -= float2(0.5, 0.5); // make 0,0 the centre, max dist 0.5

	float rad = length(uv);

	if (rad <= 0.5 && rad > 0.485) pixel = float4(0.2, 0.05, 0.05, 1.0);
	else discard;

	return pixel;
}

float4 senseLines() : SV_TARGET
{
	return float4(0.1, 0.8, 0.2, 1.0);
}