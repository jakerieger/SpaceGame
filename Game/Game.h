//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"

#include <Model.h>
#include <PostProcess.h>

#include "AntiAliasing.h"
#include "Bloom.h"
#include "Camera.h"
#include "Environment.h"
#include "GameObject.h"
#include "HUD.h"
#include "Input.h"
#include "RenderTexture.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify {
public:
    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&)            = delete;
    Game& operator=(Game&&) = default;

    Game(Game const&)            = delete;
    Game& operator=(Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize(int& width, int& height) const noexcept;

    [[nodiscard]] InputManager* GetInputManager() const {
        return m_InputManager.get();
    }

    [[nodiscard]] f32 GetFrameRate() const {
        return m_FrameRate;
    }

private:
    void Update(DX::StepTimer const& timer);
    void Render();

    void ScenePass(ID3D11DeviceContext* context);
    void PostProcessPass(ID3D11DeviceContext* context);

    void ProcessTonemap(ID3D11DeviceContext* context);

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    void AddGameObject(IGameObject* go) {
        m_GameObjects.push_back(go);
    }

    // Device resources.
    Unique<DX::DeviceResources> m_DeviceResources;

    // Rendering loop timer.
    DX::StepTimer m_Timer;

    // Input management class
    Unique<InputManager> m_InputManager;

    // Runtime resources
    Vector<IGameObject*> m_GameObjects;
    Unique<Camera> m_MainCamera;

    // PBR pipeline resources
    Unique<DX::RenderTexture> m_HdrScene;
    Unique<DirectX::ToneMapPostProcess> m_ToneMap;
    Unique<Environment> m_Environment;
    Unique<Bloom> m_Bloom;
    Unique<AntiAliasing> m_AntiAliasing;

    // UI resource
    Unique<HUD> m_HUD;

    f32 m_FrameRate = 0.f;
};