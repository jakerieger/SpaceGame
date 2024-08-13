#include "Structures.hlsli"

Texture2D<float4> Texture : register(t0);
sampler Sampler : register(s0);

// TODO: Pass these in instead of hardcoding
const uint ScreenWidth = 1920;
const uint ScreenHeight = 1080;

float rgb2luma(float3 rgb) {
    return sqrt(dot(rgb, float3(0.299, 0.587, 0.114)));
}

float4 main(VSInputTx pin) : SV_Target0 {
    float4 color = Texture.Sample(Sampler, pin.TexCoord);
    return color;
}