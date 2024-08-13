#pragma once

#include "GameObject.h"
#include "Input.h"
#include "Environment.h"

#include <Model.h>
#include <Effects.h>
#include <CommonStates.h>

class Spaceship final : public IGameObject, public IInputListener {
public:
    explicit Spaceship(ID3D11Device* device);
    ~Spaceship() override;

    void Draw(ID3D11DeviceContext* context,
              Matrix& viewMatrix,
              Matrix& projMatrix) override;

    void SetIBLTextures(
        Environment* environment);

    void OnKey(const KeyEvent& event) override;
    void OnMouseButton(const MouseEvent& event) override;
    void OnScroll(const ScrollEvent& event) override;
    void OnMouseMove(const MouseMoveEvent& event) override;

private:
    Unique<DirectX::PBREffectFactory> m_FxFactory;
    Unique<DirectX::Model> m_Model;
    Unique<DirectX::CommonStates> m_States;

    ComPtr<ID3D11ShaderResourceView> m_Albedo;
    ComPtr<ID3D11ShaderResourceView> m_Normal;
    ComPtr<ID3D11ShaderResourceView> m_RoughnessMetallic;
    ComPtr<ID3D11ShaderResourceView> m_Emissive;
};