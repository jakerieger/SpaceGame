//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "Camera.h"
#include "Skybox.h"
#include "Spaceship.h"
#include "WidgetCrosshair.h"
#include "WidgetEngineStats.h"

#include <DDSTextureLoader.h>

#include "PlanetGenerator.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false) {
    // In order for Direct2D to work, I need to specifiy a back buffer format they can both use.
    // In this case, that was "B8G8R8A8_UNORM".
    // Microsoft recommends "R10G10B10A2_UNORM" for HDR scenes but it is incompatible with Direct2D
    // so while I might lose some accuracy in the color values, I'll sacrifice that for ease of
    // rendering a user interface.
    m_DeviceResources = std::make_unique<DX::DeviceResources>(
        /*DXGI_FORMAT_R10G10B10A2_UNORM*/ DXGI_FORMAT_B8G8R8A8_UNORM,
                                          DXGI_FORMAT_D32_FLOAT,
                                          2,
                                          D3D_FEATURE_LEVEL_10_0);
    m_DeviceResources->RegisterDeviceNotify(this);

    m_HdrScene = std::make_unique<DX::RenderTexture>(DXGI_FORMAT_R16G16B16A16_FLOAT);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND const window, const int width, const int height) {
    m_DeviceResources->SetWindow(window, width, height);

    m_InputManager = std::make_unique<InputManager>();
    m_InputManager->Initialize();

    m_DeviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_DeviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    for (const auto& go : m_GameObjects) {
        go->Start();
    }
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick() {
    const auto begin = std::chrono::high_resolution_clock::now();

    m_Timer.Tick([&]() {
        Update(m_Timer);
    });

    Render();

    for (const auto& go : m_GameObjects) {
        go->LateUpdate();
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<f32, std::milli> elapsed = end - begin;
    const auto framerate = 1000.f / elapsed.count();

    static u32 frameCount     = 0;
    static auto maxFrameCount = CAST<u32>(framerate * 10.f);

    if (frameCount >= maxFrameCount) {
        frameCount  = 0;
        m_FrameRate = framerate;
    }

    frameCount++;
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer) {
    const float dT = CAST<f32>(timer.GetElapsedSeconds());

    for (const auto& go : m_GameObjects) {
        go->Update(dT);
    }

    m_MainCamera->Update(dT);
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render() {
    // Don't try to render anything before the first Update.
    if (m_Timer.GetFrameCount() == 0) {
        return;
    }

    Clear();

    m_DeviceResources->PIXBeginEvent(L"Render Scene");
    {
        const auto context = m_DeviceResources->GetD3DDeviceContext();

        for (const auto& go : m_GameObjects) {
            auto [View, Projection] = m_MainCamera->GetMatrices();
            go->Draw(context, View, Projection);
        }

        m_DeviceResources->PIXBeginEvent(L"Bloom");
        {
            m_Bloom->Process(context, m_HdrScene.get());
        }
        m_DeviceResources->PIXEndEvent();

        m_DeviceResources->PIXBeginEvent(L"AntiAliasing(FXAA)");
        {
            m_AntiAliasing->Process(context, m_Bloom->GetProcessedScene());
        }
        m_DeviceResources->PIXEndEvent();

        m_DeviceResources->PIXBeginEvent(L"Tonemap");
        {
            // TODO: Convert this to a class like Bloom
            ProcessTonemap(context);
        }
        m_DeviceResources->PIXEndEvent();

        ID3D11ShaderResourceView* nullsrv[] = {None};
        context->PSSetShaderResources(0, 1, nullsrv);

        // Force D3D commands to finish before switching to D2D
        m_DeviceResources->GetD3DDeviceContext()->Flush();
    }
    m_DeviceResources->PIXEndEvent();

    m_DeviceResources->PIXBeginEvent(L"Render HUD");
    {
        m_HUD->Draw();
    }
    m_DeviceResources->PIXEndEvent();

    // Show the new frame.
    m_DeviceResources->Present();
}

void Game::ProcessTonemap(ID3D11DeviceContext* context) {
    const auto renderTarget = m_DeviceResources->GetRenderTargetView();
    context->OMSetRenderTargets(1, &renderTarget, None);

    m_ToneMap->SetHDRSourceTexture(m_AntiAliasing->GetProcessedScene());
    m_ToneMap->Process(context);
}

// Helper method to clear the back buffers.
void Game::Clear() {
    m_DeviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    const auto context      = m_DeviceResources->GetD3DDeviceContext();
    const auto renderTarget = m_HdrScene->GetRenderTargetView();
    const auto depthStencil = m_DeviceResources->GetDepthStencilView();

    XMVECTORF32 color;
    color.v = XMColorSRGBToRGB(Colors::Black);
    context->ClearRenderTargetView(renderTarget, color);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto const viewport = m_DeviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_DeviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated() {
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated() {
    // TODO: Game is becoming background window.
}

void Game::OnSuspending() {
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming() {
    m_Timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved() {
    auto const r = m_DeviceResources->GetOutputSize();
    m_DeviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange() {
    m_DeviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(const int width, const int height) {
    // Because of the way DeviceResources handles lifetimes I have to split
    // resetting the HUD up into a before and after portion
    if (m_HUD) {
        m_HUD->ResetRenterTarget();
    }

    if (!m_DeviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    if (m_HUD) {
        m_HUD->CreateSurface(m_DeviceResources->GetSwapChain());
    }
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept {
    // TODO: Change to desired default window size (note minimum size is 640x360).
    width  = 1280;
    height = 720;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources() {
    const auto device  = m_DeviceResources->GetD3DDevice();
    const auto context = m_DeviceResources->GetD3DDeviceContext();

    // Setup alpha blending for overlaying Direct2D surface to draw HUD / UI
    {
        D3D11_BLEND_DESC blendDesc                      = {};
        blendDesc.RenderTarget[0].BlendEnable           = TRUE;
        blendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        ID3D11BlendState* blendState = None;
        DX::ThrowIfFailed(device->CreateBlendState(&blendDesc, &blendState));
        context->OMSetBlendState(blendState, None, 0xFFFFFFFF);
        blendState->Release();
    }

    m_Bloom        = std::make_unique<Bloom>(DXGI_FORMAT_R16G16B16A16_FLOAT, device);
    m_AntiAliasing = std::make_unique<AntiAliasing>(DXGI_FORMAT_R16G16B16A16_FLOAT, device);

    m_Environment = std::make_unique<Environment>(device,
                                                  L"Assets/Textures/LocalStarDiffuseHDR.dds",
                                                  L"Assets/Textures/LocalStarSpecularHDR.dds");

    m_HdrScene->SetDevice(device);

    m_ToneMap = std::make_unique<ToneMapPostProcess>(device);
    m_ToneMap->SetOperator(ToneMapPostProcess::Reinhard);
    m_ToneMap->SetTransferFunction(ToneMapPostProcess::SRGB);
    m_ToneMap->SetExposure(1.f);

    // Create main camera
    {
        using namespace DirectX::SimpleMath;
        m_MainCamera = std::make_unique<Camera>(Vector3(0.f, 9.f, 0.f),
                                                Vector3(0.f, 1.f, 0.f),
                                                60.f,
                                                16.f / 9.f);
    }

    // Create game objects
    {
        const auto spaceship = new Spaceship(device);
        m_InputManager->RegisterListener(spaceship);
        spaceship->SetIBLTextures(m_Environment.get());
        AddGameObject(spaceship);

        m_MainCamera->SetFollowOffset({0.f, 12.f, -36.f});
        m_MainCamera->SetFollowTarget(spaceship);
        m_MainCamera->SetFollow(true);

        const auto skybox = new Skybox(device, context, m_Environment->GetRadiance());
        AddGameObject(skybox);

        //const auto planet = new PlanetGenerator(1, m_Environment.get(), device, context);
        //AddGameObject(planet);
    }
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources() {
    using namespace DirectX::SimpleMath;

    const auto width  = m_DeviceResources->GetOutputSize().right;
    const auto height = m_DeviceResources->GetOutputSize().bottom;

    m_HdrScene->SetWindow(m_DeviceResources->GetOutputSize());
    m_Bloom->SetWindow(m_DeviceResources->GetOutputSize());
    m_AntiAliasing->SetWindow(m_DeviceResources->GetD3DDeviceContext(),
                              m_DeviceResources->GetOutputSize());

    auto swapChain = m_DeviceResources->GetSwapChain();
    m_HUD          = std::make_unique<HUD>(swapChain);

    const auto crosshairWidget = new WidgetCrosshair({width / 2.f, height / 2.f});
    m_HUD->AddWidget(crosshairWidget);

    const auto engineStatsWidget = new WidgetEngineStats(this, {20.f, 20.f});
    m_HUD->AddWidget(engineStatsWidget);
}

void Game::OnDeviceLost() {
    m_InputManager->Shutdown();

    for (const auto& go : m_GameObjects) {
        go->Destroyed();
        delete go;
    }

    m_Environment.reset();
    m_HdrScene->ReleaseDevice();
    m_ToneMap.reset();
    m_Bloom.reset();
    m_AntiAliasing.reset();
    m_HUD.reset();
}

void Game::OnDeviceRestored() {
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion