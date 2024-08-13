TextureCube CubeMap : register(t0);
SamplerState Sampler : register(s0);

float4 main(float3 texCoord : TEXCOORD0) : SV_TARGET0 {
    return CubeMap.Sample(Sampler, normalize(texCoord));
}