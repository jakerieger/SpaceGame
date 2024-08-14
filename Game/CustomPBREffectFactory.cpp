#include "pch.h"
#include "CustomPBREffectFactory.h"

#include "CustomPBREffect.h"

Shared<DirectX::IEffect> CustomPBREffectFactory::CreateEffect(const EffectInfo& info,
                                                              ID3D11DeviceContext* deviceContext) {
    auto effect = std::make_shared<CustomPBREffect>(m_Device);

    return effect;
}

void CustomPBREffectFactory::CreateTexture(const wchar_t* name,
                                           ID3D11DeviceContext* deviceContext,
                                           ID3D11ShaderResourceView** textureView) {}