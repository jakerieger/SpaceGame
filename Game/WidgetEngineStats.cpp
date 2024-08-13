#include "pch.h"
#include "WidgetEngineStats.h"
#include "Game.h"

void WidgetEngineStats::Draw(HUDContext context) {
    if (!m_Game) {
        return;
    }

    ID2D1SolidColorBrush* brush = None;
    DX::ThrowIfFailed(
        context.RenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brush));

    auto fps = m_Game->GetFrameRate();
    auto fmt = std::format("{:.2f} fps", fps);
    std::wstring fpsStr;
    ANSIToWide(fmt, fpsStr);
    context.RenderTarget->DrawText(fpsStr.c_str(),
                                   wcslen(fpsStr.c_str()),
                                   context.TextFormat,
                                   D2D1::RectF(20.f, 20.f, 300.f, 100.f),
                                   brush);

    auto time = (1.f / fps) * 1000.f; // in milliseconds
    fmt       = std::format("{:.4f} ms", time);
    std::wstring timeStr;
    ANSIToWide(fmt, timeStr);
    context.RenderTarget->DrawText(timeStr.c_str(),
                                   wcslen(timeStr.c_str()),
                                   context.TextFormat,
                                   D2D1::RectF(20.f, 40.f, 300.f, 100.f),
                                   brush);

    brush->Release();
}