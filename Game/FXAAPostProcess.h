#pragma once

#include <BufferHelpers.h>
#include <CommonStates.h>
#include <PostProcess.h>

class FXAAPostProcess final : public DirectX::IPostProcess {
public:
    explicit FXAAPostProcess(ID3D11Device* device);
    void Process(ID3D11DeviceContext* deviceContext, std::function<void()> setCustomState) override;
    void SetSourceTexture(ID3D11ShaderResourceView* source);
    void UpdateScreenSize(ID3D11DeviceContext* deviceContext, f32 width, f32 height);

private:
    ComPtr<ID3D11VertexShader> m_VertexShader;
    ComPtr<ID3D11PixelShader> m_PixelShader;
    Unique<DirectX::CommonStates> m_States;
    ID3D11ShaderResourceView* m_Texture = None;

    struct __declspec(align(16)) FXAAConstants {
        DirectX::XMFLOAT2 ScreenSize;
    };

    Unique<DirectX::ConstantBuffer<FXAAConstants>> m_CBuffer;
};