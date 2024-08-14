#include "Structures.hlsli"
#include "PostProcessingBuffers.hlsli"

Texture2D Texture : register(t0);
sampler Sampler : register(s0);

float4 FXAA(Texture2D tex, SamplerState samp, float2 texCoord, float2 screenSize) {
    return tex.Sample(samp, texCoord);
}

float4 main(VSInputTx pin) : SV_Target0 {
    return FXAA(Texture, Sampler, pin.TexCoord, ScreenSize);
}