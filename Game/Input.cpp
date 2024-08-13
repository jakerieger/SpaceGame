#include "pch.h"
#include "Input.h"

using namespace DirectX;

InputManager::InputManager() : m_Running(false), m_Paused(false) {}

void InputManager::Initialize() {
    SetRunning(true);

    m_DispatchThread = std::thread([this] {
        this->Dispatch();
    });
}

void InputManager::RegisterListener(IInputListener* listener) {
    if (!listener) {
        return;
    }

    m_Listeners.push_back(listener);
}

void InputManager::UnregisterListener(const IInputListener* listener) {
    if (!listener) {
        return;
    }

    const auto it = std::ranges::find_if(m_Listeners,
                                         [&](const IInputListener* list) {
                                             return listener == list;
                                         });
    if (it != m_Listeners.end()) {
        const auto index      = std::distance(m_Listeners.begin(), it);
        m_Listeners.at(index) = nullptr;
        Utilities::RemoveAt(m_Listeners, CAST<i32>(index));
    }
}

void InputManager::Shutdown() {
    SetRunning(false);
    m_DispatchThread.join();
}

void InputManager::EmitKeyDown(const u32 keyCode) {
    m_KeyStates[keyCode].Pressed  = true;
    m_KeyStates[keyCode].Released = false;

    for (const auto& listener : m_Listeners) {
        listener->OnKeyDown({keyCode});
    }
}

void InputManager::EmitKeyUp(const u32 keyCode) {
    m_KeyStates[keyCode].Pressed  = false;
    m_KeyStates[keyCode].Released = true;

    for (const auto& listener : m_Listeners) {
        listener->OnKeyUp({keyCode});
    }
}

void InputManager::EmitMouseButtonDown(const u32 button) {
    m_MouseButtonStates[button].Pressed  = true;
    m_MouseButtonStates[button].Released = false;

    for (const auto& listener : m_Listeners) {
        listener->OnMouseDown({button});
    }
}

void InputManager::EmitMouseButtonUp(const u32 button) {
    m_MouseButtonStates[button].Pressed  = false;
    m_MouseButtonStates[button].Released = true;

    for (const auto& listener : m_Listeners) {
        listener->OnMouseUp({button});
    }
}

void InputManager::EmitMouseMove(const f32 logicalX,
                                 const f32 logicalY,
                                 const f32 deltaX,
                                 const f32 deltaY) {
    for (const auto& listener : m_Listeners) {
        listener->OnMouseMove(MouseMoveEvent{logicalX, logicalY, deltaX, deltaY});
    }
}

void InputManager::EmitMouseScroll(const f32 delta) {
    for (const auto& listener : m_Listeners) {
        listener->OnScroll({delta});
    }
}

/// This handles dispatching input events that are being held down
void InputManager::Dispatch() {
    while (m_Running) {
        if (!m_Paused) {
            for (const auto& [key, state] : m_KeyStates) {
                if (state.Pressed) {
                    const KeyEvent event{key};
                    for (const auto& listener : m_Listeners) {
                        listener->OnKey(event);
                    }
                }
            }

            for (const auto& [button, state] : m_MouseButtonStates) {
                if (state.Pressed) {
                    const MouseEvent event{button};
                    for (const auto& listener : m_Listeners) {
                        listener->OnMouseButton(event);
                    }
                }
            }

            // Avoid busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(8));
        }
    }
}