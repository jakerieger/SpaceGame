#include "pch.h"
#include "SkyboxEffect.h"
#include "Skybox.h"

#pragma region Shaders
#include "Shaders/Compiled/SkyboxEffect_PS.inc"
#include "Shaders/Compiled/SkyboxEffect_VS.inc"
#pragma endregion

using namespace DirectX;
using DX::ThrowIfFailed;

namespace {
    constexpr u32 kDirtyConstantBuffer = 0x1;
    constexpr u32 kDirtyMVPMatrix      = 0x2;
}

SkyboxEffect::SkyboxEffect(ID3D11Device* device) : m_DirtyFlags(CAST<u32>(-1)),
                                                   m_ConstBuffer(device) {
    static_assert((sizeof(SkyboxEffectConstants) % 16) == 0, "CB size alignment");

    constexpr auto vsBlobSize = std::size(SkyboxEffect_VS);
    m_VSBlob                  = Vector<u8>(SkyboxEffect_VS, SkyboxEffect_VS + vsBlobSize);

    ThrowIfFailed(device->CreateVertexShader(m_VSBlob.data(),
                                             m_VSBlob.size(),
                                             nullptr,
                                             m_VS.ReleaseAndGetAddressOf()));

    constexpr auto psBlobSize = std::size(SkyboxEffect_PS);
    const auto psBlob         = Vector<u8>(SkyboxEffect_PS, SkyboxEffect_PS + psBlobSize);

    ThrowIfFailed(device->CreatePixelShader(psBlob.data(),
                                            psBlob.size(),
                                            nullptr,
                                            m_PS.ReleaseAndGetAddressOf()));
}

void SkyboxEffect::Apply(ID3D11DeviceContext* deviceContext) {
    if (m_DirtyFlags & kDirtyMVPMatrix) {
        XMMATRIX view = m_View;
        view.r[3]     = g_XMIdentityR3;
        m_MVP         = XMMatrixMultiply(view, m_Proj);

        m_DirtyFlags &= ~kDirtyMVPMatrix;
        m_DirtyFlags |= kDirtyConstantBuffer;
    }

    if (m_DirtyFlags & kDirtyConstantBuffer) {
        SkyboxEffectConstants constants;
        constants.WorldViewProj = XMMatrixTranspose(m_MVP);
        m_ConstBuffer.SetData(deviceContext, constants);

        m_DirtyFlags &= ~kDirtyConstantBuffer;
    }

    const auto cb = m_ConstBuffer.GetBuffer();
    deviceContext->VSSetConstantBuffers(0, 1, &cb);
    deviceContext->PSSetShaderResources(0, 1, m_Texture.GetAddressOf());

    deviceContext->PSSetShaderResources(0, 1, m_Texture.GetAddressOf());
    deviceContext->VSSetShader(m_VS.Get(), nullptr, 0);
    deviceContext->PSSetShader(m_PS.Get(), nullptr, 0);
}

void SkyboxEffect::GetVertexShaderBytecode(void const** pShaderByteCode, size_t* pByteCodeLength) {
    assert(pShaderByteCode != nullptr && pByteCodeLength != nullptr);
    *pShaderByteCode = m_VSBlob.data();
    *pByteCodeLength = m_VSBlob.size();
}

void SkyboxEffect::SetTexture(ID3D11ShaderResourceView* value) {
    m_Texture = value;
}

void SkyboxEffect::SetWorld(DirectX::FXMMATRIX) {
    /* Skybox doesn't use the world matrix by design */
}

void SkyboxEffect::SetView(DirectX::FXMMATRIX value) {
    m_View = value;
    m_DirtyFlags |= kDirtyMVPMatrix;
}

void SkyboxEffect::SetProjection(DirectX::FXMMATRIX value) {
    m_Proj = value;
    m_DirtyFlags |= kDirtyMVPMatrix;
}

void SkyboxEffect::SetMatrices(DirectX::FXMMATRIX,
                               DirectX::CXMMATRIX view,
                               DirectX::CXMMATRIX projection) {
    /* Skybox doesn't use the world matrix by design */
    m_View = view;
    m_Proj = projection;
    m_DirtyFlags |= kDirtyMVPMatrix;
}