float4 main(float3 posMS : modelPosition) : SV_TARGET
{
	float height = posMS.y;
	float4 color = float4(0.3, height, 0.4, 1.0);

	return color;
}
