cbuffer sceneConst : register (b0)
{
	float4 cameraPosition;
	float4 pointLightPosition;
	float4 pointLightColor;
};

float4 main(float3 posWS : WSPosition, float3 norWS : WSNormal, float4 col : Color) : SV_TARGET
{
	float4 light = 0.0;

	float3 nor = normalize(norWS);

	// ambient light
	//light = ambientSceneColor * ambientColor;

	// direcitonal light
	//light += max(0, -dot(nor, lightDirection.xyz)) * lightDirecitonColor * diffuseColor;

	// point light
	float3 toPL = pointLightPosition.xyz - posWS.xyz;

	float plNorDot = dot(nor, normalize(toPL));

	if (plNorDot > 0.0) // ignore if it is facing away
	{
		// light from point light
		float len = length(toPL);
		float att = 1.0 / (1.0 + 0.045 * len + 0.0075 * len * len);
		light += att * plNorDot * pointLightColor * col;
		//light += att * plNorDot * pointLightColor * diffuseColor;

		// point light specular
		float3 toCam = normalize(cameraPosition.xyz - posWS);
		float3 reflected = normalize(2.0 * dot(nor, toPL) * nor - toPL);
		float spec = saturate(dot(reflected, toCam));
		const float specularExponent = 5.0;
		spec = pow(spec, specularExponent); // *specIntensity;
		light += spec * pointLightColor * col;
		//light += spec * pointLightColor * specularColor;
	}

	return saturate(light);
}