#pragma once

#include "Widget.h"

class HUD {
public:
    explicit HUD(IDXGISwapChain* swapChain);
    ~HUD();
    void Draw();
    void CreateSurface(IDXGISwapChain* swapChain);
    void ResetRenterTarget();

    [[nodiscard]] Vector<IWidget*> GetWidgets() const {
        return m_Widgets;
    }

    void AddWidget(IWidget* widget) {
        m_Widgets.push_back(widget);
    }

private:
    // Direct2D resources
    ComPtr<ID2D1Factory> m_Factory;
    ComPtr<ID2D1RenderTarget> m_RenderTarget;
    ComPtr<IDWriteFactory> m_WriteFactory;
    ComPtr<IDWriteTextFormat> m_TextFormat;

    Vector<IWidget*> m_Widgets;
};