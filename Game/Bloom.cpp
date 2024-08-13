#include "pch.h"
#include "Bloom.h"

using namespace DirectX;

Bloom::Bloom(const DXGI_FORMAT format, ID3D11Device* device)
    : m_Format(format) {
    m_Basic = std::make_unique<BasicPostProcess>(device);
    m_Dual  = std::make_unique<DualPostProcess>(device);

    m_PassOne   = std::make_unique<DX::RenderTexture>(format);
    m_PassTwo   = std::make_unique<DX::RenderTexture>(format);
    m_PassThree = std::make_unique<DX::RenderTexture>(format);
    m_PassFour  = std::make_unique<DX::RenderTexture>(format);

    m_PassOne->SetDevice(device);
    m_PassTwo->SetDevice(device);
    m_PassThree->SetDevice(device);
    m_PassFour->SetDevice(device);
}

void Bloom::Process(ID3D11DeviceContext* context, const DX::RenderTexture* sceneIn) {
    auto passOne   = m_PassOne->GetRenderTargetView();
    auto passTwo   = m_PassTwo->GetRenderTargetView();
    auto passThree = m_PassThree->GetRenderTargetView();
    auto passFour  = m_PassFour->GetRenderTargetView();

    // Pass one
    {
        m_Basic->SetEffect(BasicPostProcess::BloomExtract);
        m_Basic->SetBloomExtractParameter(0.4f);
        context->OMSetRenderTargets(1, &passOne, None);
        m_Basic->SetSourceTexture(sceneIn->GetShaderResourceView());
        m_Basic->Process(context);
    }

    // Pass two
    {
        m_Basic->SetEffect(BasicPostProcess::BloomBlur);
        m_Basic->SetBloomBlurParameters(true, 4.f, 1.f);
        context->OMSetRenderTargets(1, &passTwo, None);
        m_Basic->SetSourceTexture(m_PassOne->GetShaderResourceView());
        m_Basic->Process(context);
    }

    // Pass three
    {
        m_Basic->SetBloomBlurParameters(false, 4.f, 1.f);
        ID3D11ShaderResourceView* nullsrv[] = {None, None};
        context->PSSetShaderResources(0, 2, nullsrv);
        context->OMSetRenderTargets(1, &passThree, None);
        m_Basic->SetSourceTexture(m_PassTwo->GetShaderResourceView());
        m_Basic->Process(context);
    }

    // Final pass
    {
        m_Dual->SetEffect(DualPostProcess::BloomCombine);
        m_Dual->SetBloomCombineParameters(1.5f, 1.f, 1.f, 1.f);
        context->OMSetRenderTargets(1, &passFour, None);
        m_Dual->SetSourceTexture(sceneIn->GetShaderResourceView());
        m_Dual->SetSourceTexture2(m_PassThree->GetShaderResourceView());
        m_Dual->Process(context);
    }
}