#include "Structures.hlsli"

// self-created quad.
VSInputTx main(uint vI : SV_VertexId) {
    VSInputTx vout;
    
    // We use the 'big triangle' optimization so you only Draw 3 verticies instead of 4.
    float2 texCoord = float2((vI << 1) & 2, vI & 2);
    vout.TexCoord = texCoord;

    vout.Position = float4(texCoord.x * 2 - 1, -texCoord.y * 2 + 1, 0, 1);
    return vout;
}