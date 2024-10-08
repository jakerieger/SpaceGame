#include "pch.h"
#include "FXAAPostProcess.h"

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

FXAAPostProcess::FXAAPostProcess(ID3D11Device* device) {
    static_assert((sizeof(FXAAConstants) % 16) == 0, "CB size alignment");
    DX::ThrowIfFailed(device->CreateVertexShader(kVertexShader.Code,
                                                 kVertexShader.Length,
                                                 None,
                                                 m_VertexShader.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(device->CreatePixelShader(kPixelShader.Code,
                                                kPixelShader.Length,
                                                None,
                                                m_PixelShader.ReleaseAndGetAddressOf()));

    m_States  = std::make_unique<CommonStates>(device);
    m_CBuffer = std::make_unique<ConstantBuffer<FXAAConstants>>(device);
}

void FXAAPostProcess::Process(ID3D11DeviceContext* deviceContext,
                              std::function<void()> setCustomState) {
    const auto cb = m_CBuffer->GetBuffer();
    Ignore        = cb->SetPrivateData(WKPDID_D3DDebugObjectName,
                                strlen("FXAAConstantBuffer"),
                                "FXAAConstantBuffer");
    deviceContext->PSSetConstantBuffers(0, 1, &cb);
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

void FXAAPostProcess::SetSourceTexture(ID3D11ShaderResourceView* source) {
    m_Texture = source;
}

void FXAAPostProcess::UpdateScreenSize(ID3D11DeviceContext* deviceContext, f32 width, f32 height) {
    FXAAConstants constants;
    constants.ScreenSize = {width, height};
    m_CBuffer->SetData(deviceContext, constants);
}