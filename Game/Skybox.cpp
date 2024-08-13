#include "pch.h"
#include "Skybox.h"

using namespace DirectX;
using DX::ThrowIfFailed;

Skybox::Skybox(ID3D11Device* device,
               ID3D11DeviceContext* context,
               const ComPtr<ID3D11ShaderResourceView>& cubemap)
    : m_Cubemap(cubemap) {
    m_Box    = GeometricPrimitive::CreateGeoSphere(context, 2.f, 3, false);
    m_Effect = std::make_unique<SkyboxEffect>(device);
    m_Box->CreateInputLayout(m_Effect.get(), m_InputLayout.ReleaseAndGetAddressOf());
    m_Effect->SetTexture(m_Cubemap.Get());
}

void Skybox::Draw(ID3D11DeviceContext* context,
                  SimpleMath::Matrix& viewMatrix,
                  SimpleMath::Matrix& projMatrix) {
    m_Effect->SetProjection(projMatrix);
    m_Effect->SetView(viewMatrix);
    m_Box->Draw(m_Effect.get(), m_InputLayout.Get());
}