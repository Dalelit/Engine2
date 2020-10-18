cbuffer gizmoConst : register (b0)
{
	float4 lineColor;
};

float4 main(float4 col : Color) : SV_TARGET
{
	return col * lineColor;
}
