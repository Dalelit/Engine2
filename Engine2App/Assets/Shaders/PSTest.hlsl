float4 main(float3 posWS : worldPosition) : SV_TARGET
{
	float height = abs(posWS.y - floor(posWS.y));
	height = max(height, 0.1);
	height = min(height, 1.0);
	float4 color = float4(0.1, height, 0.1, 1.0);

	return color;
}
