TextureCube CubeMap : register(t0);
SamplerState Sampler : register(s0);

float3 RotateCubemap180(float3 direction) {
    direction.x = -direction.x;
    direction.z = -direction.z;

    return direction;
}

float4 SampleRotatedCubemap(TextureCube cubemap, SamplerState samplerState, float3 direction) {
    float3 rotatedDirection = RotateCubemap180(direction);
    return cubemap.Sample(
    samplerState, rotatedDirection);
}

float4 main(float3 texCoord : TEXCOORD0) : SV_TARGET0 {
    float3 direction = normalize(texCoord);
    return SampleRotatedCubemap(CubeMap, Sampler, direction);
}