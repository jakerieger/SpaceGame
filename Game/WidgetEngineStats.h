#pragma once

#include "Widget.h"

class Game;

class WidgetEngineStats : public IWidget {
public:
    explicit WidgetEngineStats(Game* game,
                               const DirectX::XMFLOAT2& position,
                               const D2D1_RECT_F& rect = {})
        : IWidget(position, rect), m_Game(game) {}

    void Draw(HUDContext context) override;

private:
    Game* m_Game;
};