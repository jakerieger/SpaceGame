#include "pch.h"
#include "HUD.h"

using DX::ThrowIfFailed;

HUD::HUD(IDXGISwapChain* swapChain) {
    ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&m_Factory)));
    ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                                      __uuidof(IDWriteFactory),
                                      &m_WriteFactory));
    // Create D2D Surface
    CreateSurface(swapChain);

    // Setup default font (may get changed later on)
    ThrowIfFailed(m_WriteFactory->CreateTextFormat(L"Chakra Petch",
                                                   nullptr,
                                                   DWRITE_FONT_WEIGHT_NORMAL,
                                                   DWRITE_FONT_STYLE_NORMAL,
                                                   DWRITE_FONT_STRETCH_NORMAL,
                                                   12.f,
                                                   L"en-us",
                                                   &m_TextFormat));

    ThrowIfFailed(m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
    ThrowIfFailed(m_TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
}

void HUD::CreateSurface(IDXGISwapChain* swapChain) {
    IDXGISurface* surface;
    ThrowIfFailed(swapChain->GetBuffer(0, IID_PPV_ARGS(&surface)));

    const D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        0,
        0);

    ThrowIfFailed(
        m_Factory->CreateDxgiSurfaceRenderTarget(surface, &props, &m_RenderTarget));
    surface->Release();
}

void HUD::ResetRenterTarget() {
    m_RenderTarget.Reset();
}

HUD::~HUD() {
    m_Widgets.clear();
    m_RenderTarget.Reset();
    m_Factory.Reset();
    m_WriteFactory.Reset();
}

void HUD::Draw() {
    if (!m_RenderTarget) {
        return;
    }

    m_RenderTarget->BeginDraw();

    for (const auto& widget : m_Widgets) {
        widget->Draw(HUDContext(m_Factory.Get(), m_RenderTarget.Get(), m_TextFormat.Get()));
    }

    ThrowIfFailed(m_RenderTarget->EndDraw());
}