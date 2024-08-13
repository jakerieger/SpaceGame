#pragma once

#include "GameObject.h"
#include "SkyboxEffect.h"

#include <GeometricPrimitive.h>

class Skybox final : public IGameObject {
public:
    Skybox(ID3D11Device* device,
           ID3D11DeviceContext* context,
           const ComPtr<ID3D11ShaderResourceView>& cubemap);

    void Draw(ID3D11DeviceContext* context,
              DirectX::SimpleMath::Matrix& viewMatrix,
              DirectX::SimpleMath::Matrix& projMatrix) override;

private:
    Unique<DirectX::GeometricPrimitive> m_Box;
    Unique<SkyboxEffect> m_Effect;
    ComPtr<ID3D11InputLayout> m_InputLayout;
    ComPtr<ID3D11ShaderResourceView> m_Cubemap;
};