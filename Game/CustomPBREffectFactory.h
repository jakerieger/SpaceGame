#pragma once

#include <Effects.h>

class CustomPBREffectFactory final : public DirectX::IEffectFactory {
public:
    explicit CustomPBREffectFactory(ID3D11Device* device) : m_Device(device) {}

    Shared<DirectX::IEffect> CreateEffect(const EffectInfo& info,
                                          ID3D11DeviceContext* deviceContext) override;

    void CreateTexture(const wchar_t* name,
                       ID3D11DeviceContext* deviceContext,
                       ID3D11ShaderResourceView** textureView) override;

private:
    ID3D11Device* m_Device;
};