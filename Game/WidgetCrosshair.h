#pragma once
#include "Widget.h"

class WidgetCrosshair final : public IWidget {
public:
    WidgetCrosshair(const DirectX::XMFLOAT2& position, const D2D1_RECT_F& rect = {})
        : IWidget(position, rect) {}

    void Draw(HUDContext context) override;
};