struct GSOutput
{
	float4 pos : SV_POSITION;
};

[maxvertexcount(12)]
void main(
	triangle float4 input[3] : SV_POSITION, 
	inout TriangleStream< GSOutput > output
)
{
	GSOutput element;
	uint i;

	for (i = 0; i < 3; i++)
	{
		element.pos = input[i] + float4(2.0, 0.0, 0.0, 0.0);
		output.Append(element);
	}
	output.RestartStrip();
	for (i = 0; i < 3; i++)
	{
		element.pos = input[i] + float4(0.0, 2.0, 0.0, 0.0);
		output.Append(element);
	}
	output.RestartStrip();
	for (i = 0; i < 3; i++)
	{
		element.pos = input[i] + float4(-2.0, 0.0, 0.0, 0.0);
		output.Append(element);
	}
	output.RestartStrip();
	for (i = 0; i < 3; i++)
	{
		element.pos = input[i] + float4(0.0, -2.0, 0.0, 0.0);
		output.Append(element);
	}
}