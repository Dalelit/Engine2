cbuffer materialConst : register (b1)
{
	float3 mat_ambient;
	float3 mat_diffuse;
	float3 mat_specular;
	float  mat_specularExponent;
	float3 mat_emission;
	float3 mat_padding;
};
