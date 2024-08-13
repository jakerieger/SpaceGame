#pragma once

#include "GameObject.h"

#include <Effects.h>
#include <GeometricPrimitive.h>

#include "Environment.h"

class PlanetGenerator : public IGameObject {
public:
    PlanetGenerator(u32 numPlanets,
                    Environment* environment,
                    ID3D11Device* device,
                    ID3D11DeviceContext* context);

    void Draw(ID3D11DeviceContext* context,
              Matrix& viewMatrix,
              Matrix& projMatrix) override;

private:
    u32 m_NumPlanets;

    Unique<DirectX::GeometricPrimitive> m_Model;
    Unique<DirectX::PBREffect> m_Effect;

    ComPtr<ID3D11InputLayout> m_InputLayout;
};