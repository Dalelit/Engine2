float specular(float3 positionWS, float3 normalWS, float3 lightPositionWS, float3 cameraPositionWS, float specularExponent)
{
	float3 toPL = lightPositionWS - positionWS;
	float normalLightDot = dot(normalWS, normalize(toPL));

	float3 toCam = normalize(cameraPositionWS - positionWS);
	float3 reflected = normalize(2.0 * dot(normalWS, toPL) * normalWS - toPL);
	float spec = saturate(dot(reflected, toCam));
	return pow(spec, specularExponent); // *specIntensity;
};

float attenuation(float distance)
{
	return 1.0 / (1.0 + 0.045 * distance + 0.0075 * distance * distance);
};

///////////////////////////////////////////////////

cbuffer sceneConst : register (b0)
{
	float4 cameraPosition;
	float4 pointLightPosition;
	float4 pointLightColor;
};

cbuffer materialConst : register (b1)
{
	float3 mat_ambient;
	float3 mat_diffuse;
	float3 mat_specular;
	float  mat_specularExponent;
	float3 mat_emission;
	float3 mat_padding;
};

float4 main(float3 posWS : WSPosition, float3 norWS : WSNormal, float4 col : Color) : SV_TARGET
{
	float4 target = 0.0;

	// Change the vertex color to match the diffuse color
	// Generally the model is a white vertex
	col *= float4(mat_diffuse, 1.0);

	norWS = normalize(norWS);
	float3 toLight = pointLightPosition.xyz - posWS.xyz;
	float norDotLight = dot(norWS, normalize(toLight));

	// If facing the light
	if (norDotLight > 0.0)
	{
		const float PI = 3.14159565359;

		// calc the light
		float att = attenuation(length(toLight)) * norDotLight;
		att /= PI;
		target += att; // defer *col, defer *pointLightColor

		// calc the specular light strength, then add it to the pixel with the specular color
		float specStrength = specular(posWS.xyz, norWS.xyz, pointLightPosition.xyz, cameraPosition.xyz, mat_specularExponent);
		target += specStrength * float4(mat_specular, 1.0); // defer *col, defer *pointLightColor

		target *= pointLightColor;
	}

	// Add ambient
	target += float4(mat_ambient, 1.0); // defer *col;

	target *= col; // do the deferred *col

	target += float4(mat_emission, 1.0);

	return target;
}
