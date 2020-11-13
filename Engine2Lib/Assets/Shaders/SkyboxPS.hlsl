TextureCube tex : register(t0);
SamplerState sam : register(s0);

float4 main(float3 posWS : WSPosition) : SV_TARGET
{
    return tex.Sample(sam, posWS);
}
