#pragma once
#pragma warning(disable: 4100)

#include <SimpleMath.h>

class IGameObject {
public:
    virtual ~IGameObject() = default;

    virtual void Start() {}
    virtual void Update(f32 dT) {}
    virtual void FixedUpdate() {}
    virtual void LateUpdate() {}
    virtual void Destroyed() {}
    virtual void Reset() {}

    virtual void Draw(ID3D11DeviceContext* context,
                      DirectX::SimpleMath::Matrix& viewMatrix,
                      DirectX::SimpleMath::Matrix& projMatrix) = 0;

    [[nodiscard]] DirectX::SimpleMath::Matrix GetModelMatrix() const {
        auto model = DirectX::SimpleMath::Matrix::Identity;
        model      = model.CreateScale(m_Scale);
        model      = model.CreateFromYawPitchRoll(m_Rotation);
        model.Translation(m_Position);

        return model;
    }

    [[nodiscard]] DirectX::SimpleMath::Vector3 GetPosition() const {
        return m_Position;
    }

    [[nodiscard]] DirectX::SimpleMath::Vector3 GetRotation() const {
        return m_Rotation;
    }

    [[nodiscard]] DirectX::SimpleMath::Vector3 GetScale() const {
        return m_Scale;
    }

    void SetPosition(const DirectX::SimpleMath::Vector3 position);
    void SetRotation(DirectX::SimpleMath::Vector3 rotation);
    void SetScale(DirectX::SimpleMath::Vector3 scale);

protected:
    DirectX::SimpleMath::Vector3 m_Position = {0.f, 0.f, 0.f};
    DirectX::SimpleMath::Vector3 m_Rotation = {0.f, 0.f, 0.f};
    DirectX::SimpleMath::Vector3 m_Scale    = {1.f, 1.f, 1.f};
};