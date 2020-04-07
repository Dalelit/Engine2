float4 main(float3 posWS : WSPosition, float3 norWS : WSNormal, float4 col : Color, float4 posSS : SV_POSITION) : SV_TARGET
{
	return col;
}