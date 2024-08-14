#pragma once

#include "FXAAPostProcess.h"
#include "RenderTexture.h"

class AntiAliasing {
public:
    AntiAliasing(DXGI_FORMAT format, ID3D11Device* device);

    void Process(ID3D11DeviceContext* context, ID3D11ShaderResourceView* sceneIn);

    void SetWindow(ID3D11DeviceContext* context, const RECT& rect) {
        m_Pass->SetWindow(rect);
        m_FXAA->UpdateScreenSize(context, CAST<f32>(rect.right), CAST<f32>(rect.bottom));
    }

    ID3D11ShaderResourceView* GetProcessedScene() {
        return m_Pass->GetShaderResourceView();
    }

private:
    DXGI_FORMAT m_Format;
    Unique<FXAAPostProcess> m_FXAA;
    Unique<DX::RenderTexture> m_Pass;
};