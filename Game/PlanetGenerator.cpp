#include "pch.h"
#include "PlanetGenerator.h"

using namespace DirectX;

PlanetGenerator::PlanetGenerator(const u32 numPlanets,
                                 Environment* environment,
                                 ID3D11Device* device,
                                 ID3D11DeviceContext* context)
    : m_NumPlanets(numPlanets) {
    m_Model  = GeometricPrimitive::CreateSphere(context, 1000.f, 36, false);
    m_Effect = std::make_unique<PBREffect>(device);
    m_Model->CreateInputLayout(m_Effect.get(), m_InputLayout.ReleaseAndGetAddressOf());

    m_Effect->SetIBLTextures(environment->GetRadiance().Get(),
                             environment->GetMipLevels(),
                             environment->GetIrradiance().Get());

    m_Effect->SetConstantRoughness(0.8f);
}

void PlanetGenerator::Draw(ID3D11DeviceContext* context, Matrix& viewMatrix, Matrix& projMatrix) {
    m_Position.z = 10000.F;

    m_Effect->SetView(viewMatrix);
    m_Effect->SetProjection(projMatrix);

    m_Model->Draw(m_Effect.get(), m_InputLayout.Get());
}