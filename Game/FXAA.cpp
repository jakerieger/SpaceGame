#include "pch.h"
#include "FXAA.h"
#include "DemandCreate.h"

#include <CommonStates.h>
#include <DirectXHelpers.h>

#include "Shaders/Compiled/FXAA_VS.inc";
#include "Shaders/Compiled/FXAA_PS.inc";

using namespace DirectX;

struct ShaderBytecode {
    void const* Code;
    size_t Length;
};

constexpr ShaderBytecode kVertexShader = {FXAA_VS, sizeof(FXAA_VS)};
constexpr ShaderBytecode kPixelShader  = {FXAA_PS, sizeof(FXAA_PS)};

FXAA::FXAA(ID3D11Device* device, ID3D11DeviceContext* context) {
    DX::ThrowIfFailed(device->CreateVertexShader(kVertexShader.Code,
                                                 kVertexShader.Length,
                                                 None,
                                                 m_VertexShader.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(device->CreatePixelShader(kPixelShader.Code,
                                                kPixelShader.Length,
                                                None,
                                                m_PixelShader.ReleaseAndGetAddressOf()));

    m_States = std::make_unique<CommonStates>(device);
}

void FXAA::Process(ID3D11DeviceContext* deviceContext, std::function<void()> setCustomState) {
    deviceContext->PSSetShaderResources(0, 1, &m_Texture);

    const auto sampler = m_States->LinearClamp();
    deviceContext->PSSetSamplers(0, 1, &sampler);

    deviceContext->OMSetBlendState(m_States->Opaque(), None, 0xFFFFFFFF);
    deviceContext->OMSetDepthStencilState(m_States->DepthNone(), 0);
    deviceContext->RSSetState(m_States->CullNone());

    deviceContext->VSSetShader(m_VertexShader.Get(), None, 0);
    deviceContext->PSSetShader(m_PixelShader.Get(), None, 0);

    deviceContext->IASetInputLayout(None);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    deviceContext->Draw(3, 0);
}

void FXAA::SetSourceTexture(ID3D11ShaderResourceView* source) {
    m_Texture = source;
}