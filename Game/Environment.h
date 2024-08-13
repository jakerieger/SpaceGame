#pragma once

class Environment {
public:
    Environment(ID3D11Device* device, const wstr& diffuseTexture, const wstr& specularTexture);
    ~Environment();

    [[nodiscard]] ComPtr<ID3D11ShaderResourceView> GetRadiance() {
        return m_Radiance;
    }

    [[nodiscard]] ComPtr<ID3D11ShaderResourceView> GetIrradiance() {
        return m_Irradiance;
    }

    [[nodiscard]] u32 GetMipLevels() const {
        return m_MipLevels;
    }

private:
    ComPtr<ID3D11ShaderResourceView> m_Radiance;
    ComPtr<ID3D11ShaderResourceView> m_Irradiance;
    u32 m_MipLevels = 0;
};