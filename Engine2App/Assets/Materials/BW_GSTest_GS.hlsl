cbuffer entityConst : register (b1)
{
	matrix cameraTransform;
	matrix entityTransform;
	matrix entityTransformRotation;
};

struct GSOutput
{
	float4 col : Color;
	float4 pos : SV_POSITION;
};

[maxvertexcount(5*3)]
void main(
	triangle float4 input[3] : SV_POSITION,
	inout TriangleStream< GSOutput > output
)
{
	GSOutput element;
	uint i;

	for (i = 0; i < 3; i++)
	{
		element.pos = input[i];
		element.pos = mul(mul(element.pos, entityTransform), cameraTransform);
		element.col = float4(1.0, 1.0, 1.0, 1.0);
		output.Append(element);
	}
	output.RestartStrip();
	for (i = 0; i < 3; i++)
	{
		element.pos = input[i] + float4(2.0, 0.0, 0.0, 0.0);
		element.pos = mul(mul(element.pos, entityTransform), cameraTransform);
		element.col = float4(1.0, 0.0, 0.0, 1.0);
		output.Append(element);
	}
	output.RestartStrip();
	for (i = 0; i < 3; i++)
	{
		element.pos = input[i] + float4(0.0, 2.0, 0.0, 0.0);
		element.pos = mul(mul(element.pos, entityTransform), cameraTransform);
		element.col = float4(0.0, 1.0, 0.0, 1.0);
		output.Append(element);
	}
	output.RestartStrip();
	for (i = 0; i < 3; i++)
	{
		element.pos = input[i] + float4(-2.0, 0.0, 0.0, 0.0);
		element.pos = mul(mul(element.pos, entityTransform), cameraTransform);
		element.col = float4(0.0, 0.0, 1.0, 1.0);
		output.Append(element);
	}
	output.RestartStrip();
	for (i = 0; i < 3; i++)
	{
		element.pos = input[i] + float4(0.0, -2.0, 0.0, 0.0);
		element.pos = mul(mul(element.pos, entityTransform), cameraTransform);
		element.col = float4(1.0, 1.0, 0.0, 1.0);
		output.Append(element);
	}
}