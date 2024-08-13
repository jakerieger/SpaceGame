cbuffer SkyboxConstants : register(b0) {
    float4x4 WorldViewProj;
}

struct VSOutput {
    float3 TexCoord : TEXCOORD0;
    float4 PositionPS : SV_Position;
};