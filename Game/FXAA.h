#pragma once

#include <CommonStates.h>
#include <PostProcess.h>

class FXAA final : public DirectX::IPostProcess {
public:
    FXAA(ID3D11Device* device, ID3D11DeviceContext* context);
    void Process(ID3D11DeviceContext* deviceContext, std::function<void()> setCustomState) override;
    void SetSourceTexture(ID3D11ShaderResourceView* source);

private:
    ComPtr<ID3D11VertexShader> m_VertexShader;
    ComPtr<ID3D11PixelShader> m_PixelShader;
    Unique<DirectX::CommonStates> m_States;
    ID3D11ShaderResourceView* m_Texture = None;
};