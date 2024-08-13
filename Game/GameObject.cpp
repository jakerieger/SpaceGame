#include "pch.h"
#include "GameObject.h"

void IGameObject::SetPosition(const DirectX::SimpleMath::Vector3 position) {
    m_Position = position;
}

void IGameObject::SetRotation(const DirectX::SimpleMath::Vector3 rotation) {
    m_Rotation = rotation;
}

void IGameObject::SetScale(const DirectX::SimpleMath::Vector3 scale) {
    m_Scale = scale;
}