float4 main(float4 col : Color, float2 uv : UV) : SV_TARGET
{
	uv = (uv * 2.0) - 1.0;
	//return float4(uv, 0.0, 1.0);

	float ray = max(0.0, 1.0 - abs(uv.x * uv.y * 50.0));
	if (ray < 0.3) discard;
	return col * ray;
}
