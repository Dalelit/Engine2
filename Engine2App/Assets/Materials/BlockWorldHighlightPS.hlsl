cbuffer highlightConst : register (b0)
{
	float4 lineColor;
};

float4 main() : SV_TARGET
{
	return lineColor;
}