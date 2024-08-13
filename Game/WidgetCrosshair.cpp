#include "pch.h"
#include "WidgetCrosshair.h"

#include <SimpleMath.h>

using DX::ThrowIfFailed;

static void
DrawCenter(ID2D1RenderTarget* renderTarget, D2D1_POINT_2F center, ID2D1SolidColorBrush* brush) {
    const auto crosshairCenter = D2D1::Ellipse(center, 2.f, 2.f);
    renderTarget->DrawEllipse(crosshairCenter, brush, 1);
}

static void
DrawLines(ID2D1RenderTarget* renderTarget, D2D1_POINT_2F center, ID2D1SolidColorBrush* brush) {
    const auto lineLeftStart = D2D1::Point2F(center.x - 8, center.y);
    const auto lineLeftEnd   = D2D1::Point2F(center.x - 2, center.y);
    renderTarget->DrawLine(lineLeftStart, lineLeftEnd, brush, 1);

    const auto lineRightStart = D2D1::Point2F(center.x + 8, center.y);
    const auto lineRightEnd   = D2D1::Point2F(center.x + 2, center.y);
    renderTarget->DrawLine(lineRightStart, lineRightEnd, brush, 1);

    const auto lineTopStart = D2D1::Point2F(center.x, center.y - 8);
    const auto lineTopEnd   = D2D1::Point2F(center.x, center.y - 2);
    renderTarget->DrawLine(lineTopStart, lineTopEnd, brush, 1);

    const auto lineBottomStart = D2D1::Point2F(center.x, center.y + 8);
    const auto lineBottomEnd   = D2D1::Point2F(center.x, center.y + 2);
    renderTarget->DrawLine(lineBottomStart, lineBottomEnd, brush, 1);
}

static void
DrawOuter(ID2D1RenderTarget* renderTarget, D2D1_POINT_2F center, ID2D1SolidColorBrush* brush) {
    brush->SetOpacity(0.4f);
    const auto crosshairOuter = D2D1::Ellipse(center, 24.f, 24.f);
    renderTarget->DrawEllipse(crosshairOuter, brush, 1);
}

static void
DrawStatBars(ID2D1Factory* factory,
             ID2D1RenderTarget* renderTarget,
             D2D1_POINT_2F center,
             ID2D1SolidColorBrush* brush) {
    constexpr auto kRadius   = 180.f;
    constexpr auto c         = DirectX::XM_2PI * kRadius;
    constexpr auto arcLength = c / 4;

    ComPtr<ID2D1PathGeometry> path;
    ThrowIfFailed(factory->CreatePathGeometry(&path));

    ComPtr<ID2D1GeometrySink> sink;
    ThrowIfFailed(path->Open(&sink));

    // Left Arc
    {
        auto startPt = D2D1::Point2F(center.x - kRadius, center.y + (arcLength / 2));
        auto endPt   = D2D1::Point2F(center.x - kRadius, center.y - (arcLength / 2));
        sink->BeginFigure(startPt, D2D1_FIGURE_BEGIN_FILLED);
        sink->AddArc(D2D1::ArcSegment(endPt,
                                      D2D1::SizeF(kRadius, kRadius),
                                      0.f,
                                      D2D1_SWEEP_DIRECTION_CLOCKWISE,
                                      D2D1_ARC_SIZE_SMALL));
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
    }

    // Right Arc
    {
        auto startPt = D2D1::Point2F(center.x + kRadius, center.y + (arcLength / 2));
        auto endPt   = D2D1::Point2F(center.x + kRadius, center.y - (arcLength / 2));
        sink->BeginFigure(startPt, D2D1_FIGURE_BEGIN_FILLED);
        sink->AddArc(D2D1::ArcSegment(endPt,
                                      D2D1::SizeF(kRadius, kRadius),
                                      0.f,
                                      D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
                                      D2D1_ARC_SIZE_SMALL));
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
    }

    ThrowIfFailed(sink->Close());

    renderTarget->DrawGeometry(path.Get(), brush, 4.f);
    path.Reset();
}

void WidgetCrosshair::Draw(const HUDContext context) {
    const auto renderTarget     = context.RenderTarget;
    ID2D1SolidColorBrush* brush = None;
    ThrowIfFailed(renderTarget->CreateSolidColorBrush(D2D1::ColorF(1.f, 1.f, 1.f, 0.9f), &brush));

    const auto center = D2D1::Point2F(m_Position.x, m_Position.y);

    DrawCenter(renderTarget, center, brush);
    DrawLines(renderTarget, center, brush);
    DrawOuter(renderTarget, center, brush);
    DrawStatBars(context.Factory, context.RenderTarget, center, brush);

    brush->Release();
}