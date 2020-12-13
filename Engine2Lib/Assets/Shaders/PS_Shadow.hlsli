cbuffer shadowConst : register (b2)
{
	float4x4 shadowCamera_viewProjectionMat;
	float4 shadowLightColor;
	float shadowBias;
}

Texture2D shadowTex : register (t1);
SamplerState shadowSmplr : register (s1);

float4 ShadowLighting(float3 positionWS)
{
	float4 posShadowSS = mul(float4(positionWS, 1.0), shadowCamera_viewProjectionMat); // world position in light screen space
	float2 shadowUV = posShadowSS.xy * 0.5 + 0.5; // convert to texture coords
	shadowUV.y = 1.0 - shadowUV.y; // invert y coords
	float shadowMapDepth = shadowTex.Sample(shadowSmplr, shadowUV).x; // get the depth from the light depth buffer
	if (posShadowSS.z < shadowMapDepth + shadowBias)
	{
		return shadowLightColor;
	}
	else
	{
		return float4(0.0, 0.0, 0.0, 1.0);
	}
}
