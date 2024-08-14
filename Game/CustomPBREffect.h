#pragma once

#include <BufferHelpers.h>
#include <Effects.h>

class CustomPBREffect final : public DirectX::IEffect, public DirectX::IEffectMatrices {
public:
    explicit CustomPBREffect(ID3D11Device* device);

    void Apply(ID3D11DeviceContext* deviceContext) override;
    void GetVertexShaderBytecode(void const** pShaderByteCode, size_t* pByteCodeLength) override;

    //void SetTexture(ID3D11ShaderResourceView* value);

    void XM_CALLCONV SetWorld(DirectX::FXMMATRIX value) override;
    void XM_CALLCONV SetView(DirectX::FXMMATRIX value) override;
    void XM_CALLCONV SetProjection(DirectX::FXMMATRIX value) override;
    void XM_CALLCONV SetMatrices(DirectX::FXMMATRIX world,
                                 DirectX::CXMMATRIX view,
                                 DirectX::CXMMATRIX projection) override;

private:
    ComPtr<ID3D11VertexShader> m_VS;
    ComPtr<ID3D11PixelShader> m_PS;
    //ComPtr<ID3D11ShaderResourceView> m_Texture;
    Vector<u8> m_VSBlob;

    Matrix m_View;
    Matrix m_Proj;
    Matrix m_MVP;

    u32 m_DirtyFlags;

    struct __declspec(align(16)) PBRConstants {
        DirectX::XMMATRIX WorldViewProj;
    };

    DirectX::ConstantBuffer<PBRConstants> m_CBuffer;
};