#include "pch.h"
#include "Spaceship.h"

#include <WICTextureLoader.h>

#include "CustomPBREffect.h"
#include "CustomPBREffectFactory.h"

using namespace DirectX;
using DX::ThrowIfFailed;

static constexpr f32 kSpeed = 5.f;

Spaceship::Spaceship(ID3D11Device* device) {
    m_FxFactory = std::make_unique<CustomPBREffectFactory>(device);

    m_Model  = Model::CreateFromSDKMESH(device, L"Assets/Models/StarShip.SDKMESH", *m_FxFactory);
    m_States = std::make_unique<CommonStates>(device);

    ThrowIfFailed(CreateWICTextureFromFile(device,
                                           L"Assets/Textures/Material.001_Base_color.jpg",
                                           None,
                                           m_Albedo.ReleaseAndGetAddressOf()));
    ThrowIfFailed(CreateWICTextureFromFile(device,
                                           L"Assets/Textures/Material.001_Normal_DirectX.jpg",
                                           None,
                                           m_Normal.ReleaseAndGetAddressOf()));
    ThrowIfFailed(CreateWICTextureFromFile(device,
                                           L"Assets/Textures/RMAO.png",
                                           None,
                                           m_RoughnessMetallic.ReleaseAndGetAddressOf()));
    ThrowIfFailed(CreateWICTextureFromFile(device,
                                           L"Assets/Textures/Material.001_Emissive.jpg",
                                           None,
                                           m_Emissive.ReleaseAndGetAddressOf()));
}

Spaceship::~Spaceship() {
    m_States.reset();
    m_Model.reset();
    m_FxFactory.reset();

    m_Albedo.Reset();
    m_Normal.Reset();
    m_RoughnessMetallic.Reset();
    m_Emissive.Reset();
}

void Spaceship::Draw(ID3D11DeviceContext* context,
                     Matrix& viewMatrix,
                     Matrix& projMatrix) {
    const auto modelMatrix = GetModelMatrix();
    m_Model->Draw(context, *m_States, modelMatrix, viewMatrix, projMatrix);
}

void Spaceship::SetIBLTextures(
    Environment* environment) {
    m_Model->UpdateEffects([&](IEffect* effect) {
        //const auto pbrEffect = DCAST<PBREffect*>(effect);
        //if (pbrEffect) {
        //    pbrEffect->SetIBLTextures(environment->GetRadiance().Get(),
        //                              environment->GetMipLevels(),
        //                              environment->GetIrradiance().Get());
        //    pbrEffect->SetSurfaceTextures(m_Albedo.Get(),
        //                                  m_Normal.Get(),
        //                                  m_RoughnessMetallic.Get());
        //    pbrEffect->SetEmissiveTexture(m_Emissive.Get());
        //}

        const auto pbrEffect = DCAST<CustomPBREffect*>(effect);

        if (!pbrEffect) {
            return;
        }
    });
}

void Spaceship::OnKey(const KeyEvent& event) {
    IInputListener::OnKey(event);
    const auto key = event.KeyCode;

    if (key == 'W') {
        m_Position.z += kSpeed;
    }

    if (key == 'S') {
        m_Position.z -= kSpeed;
    }
}

void Spaceship::OnMouseButton(const MouseEvent& event) {
    IInputListener::OnMouseButton(event);

    // left mouse button
    if (event.Button == 0) {}
}

void Spaceship::OnScroll(const ScrollEvent& event) {
    IInputListener::OnScroll(event);
}

void Spaceship::OnMouseMove(const MouseMoveEvent& event) {
    IInputListener::OnMouseMove(event);

    constexpr f32 speed = 0.1f;
    const f32 angleX    = event.DeltaX * speed;
    const f32 angleY    = event.DeltaY * speed;

    // Rotate left/right
    // TODO: Add smoothing to this
    {
        m_Rotation.z += XMConvertToRadians(angleX);

        if (m_Rotation.z >= XMConvertToRadians(90.f)) {
            m_Rotation.z = XMConvertToRadians(90.f);
        }

        if (m_Rotation.z <= XMConvertToRadians(-90.f)) {
            m_Rotation.z = XMConvertToRadians(-90.f);
        }
    }

    // Rotate up/down
    //{
    //    m_Rotation.x -= XMConvertToRadians(angleY / 2.f);

    //    if (m_Rotation.x >= XMConvertToRadians(30.f)) {
    //        m_Rotation.x = XMConvertToRadians(30.f);
    //    }

    //    if (m_Rotation.x <= XMConvertToRadians(-30.f)) {
    //        m_Rotation.x = XMConvertToRadians(-30.f);
    //    }
    //}
}