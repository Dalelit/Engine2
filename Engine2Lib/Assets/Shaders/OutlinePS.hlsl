cbuffer outlineConst : register (b2)
{
	float4 outlineColor;
};

float4 main() : SV_TARGET
{
	return outlineColor;
}