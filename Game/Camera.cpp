#include "pch.h"
#include "Camera.h"

using namespace DirectX::SimpleMath;

Camera::Camera(const Vector3 target,
               const Vector3 up,
               const f32 fovDegrees,
               const f32 aspectRatio,
               const f32 nearZ,
               const f32 farZ) {
    m_Target = target;
    m_Up     = up;
    m_FOV    = DirectX::XMConvertToRadians(fovDegrees);
    m_Aspect = aspectRatio;
    m_NearZ  = nearZ;
    m_FarZ   = farZ;
}

void Camera::Update(const f32 dT) {
    IGameObject::Update(dT);

    if (m_Follow && m_FollowTarget) {
        auto targetPos      = m_FollowTarget->GetPosition();
        const auto desiredX = targetPos.x + m_FollowOffset.x;
        const auto desiredY = targetPos.y + m_FollowOffset.y;
        const auto desiredZ = targetPos.z + m_FollowOffset.z;

        constexpr f32 smoothingFactor = 6.f;
        const f32 factor              = 1.0f - std::exp(-smoothingFactor * dT);
        m_Position.x += (desiredX - m_Position.x) * factor;
        m_Position.y += (desiredY - m_Position.y) * factor;
        m_Position.z += (desiredZ - m_Position.z) * factor;

        // this moves the target towards the bottom of the screen instead
        // of in the center
        targetPos.y += 9;
        m_Target = targetPos;
    }
}

void Camera::SetFollow(const bool follow) {
    m_Follow = follow;
}

void Camera::SetFollowOffset(const Vector3 offset) {
    m_FollowOffset = offset;
}

void Camera::SetFollowTarget(IGameObject* target) {
    m_FollowTarget = target;
}

CameraMatrices Camera::GetMatrices() {
    const auto view = Matrix::CreateLookAt(m_Position, m_Target, m_Up);
    const auto proj = Matrix::CreatePerspectiveFieldOfView(m_FOV, m_Aspect, m_NearZ, m_FarZ);

    return {view, proj};
}