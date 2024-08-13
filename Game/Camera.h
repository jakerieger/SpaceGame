#pragma once

#include "GameObject.h"

#include <SimpleMath.h>

struct CameraMatrices {
    DirectX::SimpleMath::Matrix View;
    DirectX::SimpleMath::Matrix Projection;
};

class Camera : public IGameObject {
public:
    Camera(
        DirectX::SimpleMath::Vector3 target,
        DirectX::SimpleMath::Vector3 up,
        f32 fovDegrees,
        f32 aspectRatio,
        f32 nearZ = 0.1f,
        f32 farZ  = 100000.f);

    void Draw(ID3D11DeviceContext* context,
              DirectX::SimpleMath::Matrix& viewMatrix,
              DirectX::SimpleMath::Matrix& projMatrix) override {}

    void Update(f32 dT) override;

    void SetFollow(bool follow);
    void SetFollowOffset(DirectX::SimpleMath::Vector3 offset);
    void SetFollowTarget(IGameObject* target);

    CameraMatrices GetMatrices();

private:
    DirectX::SimpleMath::Vector3 m_Target;
    DirectX::SimpleMath::Vector3 m_Up;
    f32 m_FOV;
    f32 m_Aspect;
    f32 m_NearZ;
    f32 m_FarZ;
    bool m_Follow = false;
    DirectX::SimpleMath::Vector3 m_FollowOffset;
    IGameObject* m_FollowTarget = None;
};