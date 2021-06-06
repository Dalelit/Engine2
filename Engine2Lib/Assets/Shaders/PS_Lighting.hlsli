// ensure all directions, normals are normalised
float specular(float3 positionWS, float3 normalWS, float3 toLightDirection, float3 cameraPositionWS, float specularExponent)
{
	float3 toCam = normalize(cameraPositionWS - positionWS);
	float3 reflected = reflect(toLightDirection, normalWS);
	float spec = max(dot(reflected, toCam), 0.0);
	return pow(spec, specularExponent); // *specIntensity;
};

float attenuation(float distance)
{
	return 1.0 / (1.0 + 0.045 * distance + 0.0075 * distance * distance);
};

float4 fresnelSchlick(float cosTheta, float4 F0)
{
	return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
};
