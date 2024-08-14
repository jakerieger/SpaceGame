#include "Structures.hlsli"
#include "PostProcessingBuffers.hlsli"

Texture2D Texture : register(t0);
sampler Sampler : register(s0);

float4 FXAA(Texture2D tex, SamplerState samp, float2 texCoord, float2 screenSize) {
    float2 invScreenSize = 1.0 / screenSize;
    float FXAA_SPAN_MAX = 8.0;
    float FXAA_REDUCE_MUL = 1.0 / FXAA_SPAN_MAX;
    float FXAA_REDUCE_MIN = 1.0 / 128.0;
    float FXAA_SUBPIX_SHIFT = 1.0 / 4.0;

    // Sample surrounding pixels
    float3 rgbNW = tex.Sample(samp, texCoord + (invScreenSize * float2(-1.0, -1.0))).rgb;
    float3 rgbNE = tex.Sample(samp, texCoord + (invScreenSize * float2(1.0, -1.0))).rgb;
    float3 rgbSW = tex.Sample(samp, texCoord + (invScreenSize * float2(-1.0, 1.0))).rgb;
    float3 rgbSE = tex.Sample(samp, texCoord + (invScreenSize * float2(1.0, 1.0))).rgb;
    float3 rgbM = tex.Sample(samp, texCoord).rgb;

    // Luminance computation
    float lumaNW = dot(rgbNW, float3(0.299, 0.587, 0.114));
    float lumaNE = dot(rgbNE, float3(0.299, 0.587, 0.114));
    float lumaSW = dot(rgbSW, float3(0.299, 0.587, 0.114));
    float lumaSE = dot(rgbSE, float3(0.299, 0.587, 0.114));
    float lumaM = dot(rgbM, float3(0.299, 0.587, 0.114));

    // Edge detection
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    float lumaRange = lumaMax - lumaMin;

    if (lumaRange < max(FXAA_REDUCE_MIN, lumaMax * FXAA_REDUCE_MUL)) {
        return float4(rgbM, 1.0); // No need for FXAA
    }

    // Calculate direction to blur
    float2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float2 absDir = abs(dir);
    if (absDir.x >= absDir.y) {
        dir.xy /= absDir.x;
    } else {
        dir.xy /= absDir.y;
    }

    // Apply FXAA
    float2 offset1 = FXAA_SUBPIX_SHIFT * invScreenSize;
    float2 offset2 = dir * FXAA_SPAN_MAX * invScreenSize;
    float3 result1 = 0.5 * (
        tex.Sample(samp, texCoord + dir * offset1).rgb +
        tex.Sample(samp, texCoord - dir * offset1).rgb
    );
    float3 result2 = 0.5 * (
        tex.Sample(samp, texCoord + offset2).rgb +
        tex.Sample(samp, texCoord - offset2).rgb
    );

    return float4(lerp(result1, result2, 0.5), 1.0);
}

float4 main(VSInputTx pin) : SV_Target0 {
    return FXAA(Texture, Sampler, pin.TexCoord, ScreenSize);
}