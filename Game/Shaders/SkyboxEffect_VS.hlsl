#include "SkyboxEffect_Common.hlsli"

VSOutput main(float4 position : SV_Position) {
    VSOutput vout;

    vout.PositionPS = mul(position, WorldViewProj);
    vout.PositionPS.z = vout.PositionPS.w; // draw on far plane
    vout.TexCoord = position.xyz;

    return vout;
}