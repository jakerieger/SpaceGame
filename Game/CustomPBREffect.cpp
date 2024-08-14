#include "pch.h"
#include "CustomPBREffect.h"

CustomPBREffect::CustomPBREffect(ID3D11Device* device) {}

void CustomPBREffect::Apply(ID3D11DeviceContext* deviceContext) {}

void CustomPBREffect::
GetVertexShaderBytecode(void const** pShaderByteCode, size_t* pByteCodeLength) {}

void CustomPBREffect::SetWorld(DirectX::FXMMATRIX value) {}

void CustomPBREffect::SetView(DirectX::FXMMATRIX value) {}

void CustomPBREffect::SetProjection(DirectX::FXMMATRIX value) {}

void CustomPBREffect::SetMatrices(DirectX::FXMMATRIX world,
                                  DirectX::CXMMATRIX view,
                                  DirectX::CXMMATRIX projection) {
    IEffectMatrices::SetMatrices(world, view, projection);
}