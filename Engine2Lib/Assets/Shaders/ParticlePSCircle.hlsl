float4 main(float4 col : Color, float2 uv : UV) : SV_TARGET
{
	uv = (uv * 2.0) - 1.0;
	float dist = uv.x * uv.x + uv.y * uv.y;
	if (dist > 1.0) discard;
	return col;
}
