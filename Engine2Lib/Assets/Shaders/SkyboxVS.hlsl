cbuffer sceneConst : register (b0)
{
	matrix cameraTransform;
};


struct VSOut
{
	float3 posWS : WSPosition;
	float4 posSS : SV_POSITION;
};

VSOut main( float3 pos : Position)
{
    VSOut vso;
    
    vso.posWS = pos;
    vso.posSS = mul(float4(pos, 0.0), cameraTransform);

    // make sure that the depth after w divide will be 1.0
    vso.posSS.z = vso.posSS.w;
    return vso;
}
