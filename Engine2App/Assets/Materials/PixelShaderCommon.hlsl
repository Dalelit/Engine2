float4 fresnelSchlick(float cosTheta, float4 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float specular(float3 positionWS, float3 normalWS, float3 lightPositionWS, float3 cameraPositionWS, float specularExponent)
{
	float3 toPL = lightPositionWS - positionWS;
	float normalLightDot = dot(normalWS, normalize(toPL));

	float3 toCam = normalize(cameraPositionWS - positionWS);
	float3 reflected = normalize(2.0 * dot(normalWS, toPL) * normalWS - toPL);
	float spec = saturate(dot(reflected, toCam));
	return pow(spec, specularExponent); // *specIntensity;
}

