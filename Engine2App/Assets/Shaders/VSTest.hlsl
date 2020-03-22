struct VSOut
{
	float4 col : Color;
	float4 pos : SV_POSITION;
};

VSOut main(float3 pos : Position, float4 col : Color)
{
	VSOut vso;

	vso.pos = float4(pos, 1.0f);
	vso.col = col;

	return vso;
}