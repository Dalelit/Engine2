float4 main(float3 col : Color, float2 uv : UV) : SV_TARGET
{
	uv = (uv * 2.0) - 1.0;
	float dist = uv.x * uv.x + uv.y * uv.y;
	if (dist > 1.0) discard;
	return float4(col.xy, uv.x, 1.0);
}
