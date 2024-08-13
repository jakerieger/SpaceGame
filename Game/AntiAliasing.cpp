#include "pch.h"

#include "AntiAliasing.h"

using namespace DirectX;

AntiAliasing::AntiAliasing(DXGI_FORMAT format, ID3D11Device* device) : m_Format(format) {
    m_FXAA = std::make_unique<FXAAPostProcess>(device);
    m_Pass = std::make_unique<DX::RenderTexture>(format);
    m_Pass->SetDevice(device);
}

void AntiAliasing::Process(ID3D11DeviceContext* context, ID3D11ShaderResourceView* sceneIn) {
    const auto pass = m_Pass->GetRenderTargetView();

    context->OMSetRenderTargets(1, &pass, None);
    m_FXAA->SetSourceTexture(sceneIn);
    m_FXAA->Process(context, {});
}