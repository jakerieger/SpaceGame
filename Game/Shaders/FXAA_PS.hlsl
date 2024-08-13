#include "Structures.hlsli"

Texture2D<float4> Texture : register(t0);
sampler Sampler : register(s0);

float4 main(VSInputTx pin) : SV_Target0 {
    float4 color = Texture.Sample(Sampler, pin.TexCoord);
    return color;
}