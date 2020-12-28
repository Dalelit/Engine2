cbuffer shadowConst : register (b2)
{
	float4x4 shadowCamera_viewProjectionMat;
	float4 shadowLightColor;
	float4 shadowLightDirection;
	float shadowBias;
}

Texture2D shadowTex : register (t1);
SamplerComparisonState shadowSS : register (s1);

float4 ShadowLighting(float3 positionWS, float3 normalWS)
{
	float4 result = float4(0.0, 0.0, 0.0, 1.0);
	float dotNormLight = dot(normalWS, shadowLightDirection.xyz);
	if (dotNormLight < 0.0)
	{
		float4 posShadowSS = mul(float4(positionWS, 1.0), shadowCamera_viewProjectionMat); // world position in light screen space
		float2 shadowUV = posShadowSS.xy * 0.5 + 0.5; // convert to texture coords
		shadowUV.y = 1.0 - shadowUV.y; // invert y coords

		const int range = 2;
		const float width = float(range + range + 1);
		const float areaInv = 1.0 / (width * width); // pre compute area
		
		float shadowMapDepth = 0.0;

		[unroll]
		for (int x = -range; x <= range; x++)
		{
			[unroll]
			for (int y = -range; y <= range; y++)
			{
				shadowMapDepth += shadowTex.SampleCmpLevelZero(shadowSS, shadowUV, posShadowSS.z - shadowBias, int2(x,y)).x; // sample the depth from the light depth buffer
			}
		}

		shadowMapDepth *= areaInv; // average across area

		dotNormLight = -dotNormLight;
		result = shadowLightColor * dotNormLight * shadowMapDepth;
	}

	return result;
}
