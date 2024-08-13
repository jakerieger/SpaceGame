#pragma once

struct HUDContext {
    ID2D1Factory* Factory           = None;
    ID2D1RenderTarget* RenderTarget = None;
    IDWriteTextFormat* TextFormat   = None;
};

class IWidget {
public:
    IWidget(const DirectX::XMFLOAT2 position, const D2D1_RECT_F rect) : m_Position(position),
        m_Rect(rect) {}

    virtual ~IWidget() = default;

    virtual void Draw(HUDContext context) = 0;

protected:
    DirectX::XMFLOAT2 m_Position;
    D2D1_RECT_F m_Rect;
};