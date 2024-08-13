#pragma once
#pragma warning(disable: 4100)

#include <unordered_map>
#include <thread>

struct KeyState {
    bool Pressed  = false;
    bool Released = false;
};

struct MouseButtonState {
    bool Pressed  = false;
    bool Released = false;
};

struct KeyEvent {
    u32 KeyCode;
    u32 Modifiers;
};

struct MouseEvent {
    u32 Button;
};

struct MouseMoveEvent {
    f32 LogicalX;
    f32 LogicalY;
    f32 DeltaX;
    f32 DeltaY;
};

struct ScrollEvent {
    f64 Delta;
};

/// Generic interface game objects can implement to receive input events
class IInputListener {
public:
    virtual ~IInputListener() = default;

    virtual void OnMouseDown(const MouseEvent& event) {}
    virtual void OnMouseUp(const MouseEvent& event) {}
    virtual void OnMouseButton(const MouseEvent& event) {}
    virtual void OnMouseMove(const MouseMoveEvent& event) {}
    virtual void OnKeyDown(const KeyEvent& event) {}
    virtual void OnKeyUp(const KeyEvent& event) {}
    virtual void OnKey(const KeyEvent& event) {}
    virtual void OnScroll(const ScrollEvent& event) {}
};

/// Handles dispatching input events to listerns. Managed by the game instance.
class InputManager {
public:
    InputManager();

    void Initialize();
    void RegisterListener(IInputListener* listener);
    void UnregisterListener(const IInputListener* listener);
    void Shutdown();

    void EmitKeyDown(u32 keyCode);
    void EmitKeyUp(u32 keyCode);
    void EmitMouseButtonDown(u32 button);
    void EmitMouseButtonUp(u32 button);
    void EmitMouseMove(f32 logicalX, f32 logicalY, f32 deltaX, f32 deltaY);
    void EmitMouseScroll(f32 delta);

    void SetRunning(const bool running) {
        m_Running = running;
    }

    void SetPaused(const bool paused) {
        m_Paused = paused;
    }

private:
    void Dispatch();

    Vector<IInputListener*> m_Listeners;
    std::unordered_map<u32, KeyState> m_KeyStates;
    std::unordered_map<u32, MouseButtonState> m_MouseButtonStates;
    std::thread m_DispatchThread;

    bool m_Running;
    bool m_Paused;
};