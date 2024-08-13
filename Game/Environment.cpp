#include "pch.h"
#include "Environment.h"

#include <DDSTextureLoader.h>

using namespace DirectX;
using DX::ThrowIfFailed;

Environment::Environment(ID3D11Device* device,
                         const wstr& diffuseTexture,
                         const wstr& specularTexture) {
    ThrowIfFailed(CreateDDSTextureFromFile(device,
                                           diffuseTexture.c_str(),
                                           None,
                                           m_Irradiance.ReleaseAndGetAddressOf()));
    ThrowIfFailed(CreateDDSTextureFromFile(device,
                                           specularTexture.c_str(),
                                           None,
                                           m_Radiance.ReleaseAndGetAddressOf()));

    D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
    m_Radiance->GetDesc(&desc);

    m_MipLevels = desc.TextureCube.MipLevels;
}

Environment::~Environment() {
    m_Radiance.Reset();
    m_Irradiance.Reset();
}