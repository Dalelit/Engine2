Texture2D tex;
SamplerState smplr;


float4 main(float3 posMS : modelPosition) : SV_TARGET
{
	float4 result;

	if (posMS.x < 0.00001 || posMS.x > 0.99999) result = float4(tex.Sample(smplr, posMS.yz).rgb, 1.0f);
	else if (posMS.y < 0.00001 || posMS.y > 0.99999) result = float4(tex.Sample(smplr, posMS.xz).rgb, 1.0f);
	else result = float4(tex.Sample(smplr, posMS.xy).rgb, 1.0f);

	//float4 result = float4(0.6f, posMS.y * 0.6f, 0.2f, 1.0);

	return result;
}
