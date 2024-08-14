#pragma once
#include <array>
#include <PostProcess.h>

#include "RenderTexture.h"

class Bloom {
public:
    Bloom(DXGI_FORMAT format, ID3D11Device* device);

    void Process(ID3D11DeviceContext* context, ID3D11ShaderResourceView* sceneIn);

    void SetWindow(const RECT& rect) {
        m_PassOne->SetWindow(rect);
        m_PassTwo->SetWindow(rect);
        m_PassThree->SetWindow(rect);
        m_PassFour->SetWindow(rect);
    }

    ID3D11ShaderResourceView* GetProcessedScene() {
        return m_PassFour->GetShaderResourceView();
    }

private:
    DXGI_FORMAT m_Format;
    Unique<DirectX::BasicPostProcess> m_Basic;
    Unique<DirectX::DualPostProcess> m_Dual;

    Unique<DX::RenderTexture> m_PassOne;
    Unique<DX::RenderTexture> m_PassTwo;
    Unique<DX::RenderTexture> m_PassThree;
    Unique<DX::RenderTexture> m_PassFour;
};