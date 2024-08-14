//
// Main.cpp
//

#include "pch.h"
#include "Game.h"

using namespace DirectX;

#ifdef __clang__
    #pragma clang diagnostic ignored "-Wcovered-switch-default"
    #pragma clang diagnostic ignored "-Wswitch-enum"
#endif

#pragma warning(disable : 4061)

namespace {
    std::unique_ptr<Game> g_Game;
    POINT g_LogicalCursorPosition = {0, 0};
}

auto g_AppName = L"Space Game <DX11>";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ExitGame() noexcept;

// Indicates to hybrid graphics systems to prefer the discrete part by default 
extern "C" {
__declspec(dllexport) DWORD NvOptimusEnablement                = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance,
                    _In_opt_ HINSTANCE hPrevInstance,
                    _In_ LPWSTR lpCmdLine,
                    _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (!XMVerifyCPUSupport())
        return 1;

    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr))
        return 1;

    g_Game = std::make_unique<Game>();

    HWND hwnd = None;

    // Register class and create window
    {
        // Register class
        WNDCLASSEXW wcex   = {};
        wcex.cbSize        = sizeof(WNDCLASSEXW);
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc   = WndProc;
        wcex.hInstance     = hInstance;
        wcex.hIcon         = LoadIconW(hInstance, L"IDI_ICON");
        wcex.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
        wcex.hbrBackground = RCAST<HBRUSH>(COLOR_WINDOW + 1);
        wcex.lpszClassName = L"GameWindowClass";
        wcex.hIconSm       = LoadIconW(wcex.hInstance, L"IDI_ICON");
        if (!RegisterClassExW(&wcex))
            return 1;

        // Create window
        int w, h;
        g_Game->GetDefaultSize(w, h);

        RECT rc = {0, 0, static_cast<LONG>(w), static_cast<LONG>(h)};

        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        hwnd = CreateWindowExW(0,
                               L"GameWindowClass",
                               g_AppName,
                               WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               rc.right - rc.left,
                               rc.bottom - rc.top,
                               nullptr,
                               nullptr,
                               hInstance,
                               g_Game.get());
        // TODO: Change to CreateWindowExW(WS_EX_TOPMOST, L"GameWindowClass", g_AppName, WS_POPUP,
        // to default to fullscreen.

        if (!hwnd)
            return 1;

        ShowWindow(hwnd, nCmdShow);
        // TODO: Change nCmdShow to SW_SHOWMAXIMIZED to default to fullscreen.

        // Keeps the cursor within the bounds of the Window.
        GetClientRect(hwnd, &rc);
        MapWindowPoints(hwnd, None, RCAST<LPPOINT>(&rc), 2);
        ClipCursor(&rc);

        g_Game->Initialize(hwnd, rc.right - rc.left, rc.bottom - rc.top);
    }

    // Main message loop
    MSG msg = {};
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            g_Game->Tick();
        }
    }

    g_Game.reset();

    CoUninitialize();

    return CAST<int>(msg.wParam);
}

struct WindowState {
    bool SizeMove   = false;
    bool Suspended  = false;
    bool Minimized  = false;
    bool Fullscreen = false;
};

static LRESULT OnCreate(HWND hwnd, LPARAM lParam);
static LRESULT OnPaint(HWND hwnd, Game* game, bool sizeMove);
static LRESULT OnDisplayChange(Game* game);
static LRESULT OnMove(Game* game);
static LRESULT OnSize(Game* game,
                      WindowState& windowState,
                      LPARAM lParam,
                      WPARAM wParam);
static LRESULT OnSizeMove(WindowState& windowState,
                          bool isEnter = true,
                          Game* game   = nullptr,
                          HWND hwnd    = nullptr);
static LRESULT OnMinMaxInfo(LPARAM lParam);
static LRESULT OnActivateApp(Game* game, WPARAM wParam);
static LRESULT OnPowerBroadcast(WindowState& windowState, Game* game, WPARAM wParam);
static LRESULT OnKeyDown(Game* game, WPARAM wParam);
static LRESULT OnSysKeyDown(LPARAM lParam,
                            WPARAM wParam,
                            WindowState& windowState,
                            Game* game,
                            HWND hwnd);

// Windows procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static WindowState windowState = {};
    const auto game                = RCAST<Game*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message) {
        case WM_CREATE:
            return OnCreate(hWnd, lParam);

        case WM_PAINT:
            return OnPaint(hWnd, game, windowState.SizeMove);

        case WM_DISPLAYCHANGE:
            return OnDisplayChange(game);

        case WM_MOVE:
            return OnMove(game);

        case WM_SIZE:
            return OnSize(game, windowState, lParam, wParam);

        case WM_ENTERSIZEMOVE:
            return OnSizeMove(windowState, true);

        case WM_EXITSIZEMOVE:
            return OnSizeMove(windowState, false, game, hWnd);

        case WM_GETMINMAXINFO:
            return OnMinMaxInfo(lParam);

        case WM_ACTIVATEAPP:
            return OnActivateApp(game, wParam);

        case WM_POWERBROADCAST:
            return OnPowerBroadcast(windowState, game, wParam);

        case WM_CLOSE:
            game->OnDeviceLost();
            PostQuitMessage(0);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_KEYDOWN:
            return OnKeyDown(game, wParam);

        case WM_KEYUP:
            if (game)
                game->GetInputManager()->EmitKeyUp(CAST<u32>(wParam));
            break;

        case WM_MOUSEMOVE: {
            POINT currentPos;
            GetCursorPos(&currentPos);
            ScreenToClient(hWnd, &currentPos);
            RECT rc;
            GetClientRect(hWnd, &rc);
            const f32 centerX = (rc.right - rc.left) / 2.f;
            const f32 centerY = (rc.bottom - rc.top) / 2.f;
            const f32 deltaX  = currentPos.x - centerX;
            const f32 deltaY  = currentPos.y - centerY;

            g_LogicalCursorPosition.x += CAST<LONG>(deltaX);
            g_LogicalCursorPosition.y += CAST<LONG>(deltaY);

            if (game)
                game->GetInputManager()->EmitMouseMove(CAST<f32>(g_LogicalCursorPosition.x),
                                                       CAST<f32>(g_LogicalCursorPosition.y),
                                                       deltaX,
                                                       deltaY);

            SetCursorPos(CAST<i32>(centerX), CAST<i32>(centerY));
        }
        break;

        case WM_LBUTTONDOWN: {
            if (game)
                game->GetInputManager()->EmitMouseButtonDown(0);
        }
        break;

        case WM_LBUTTONUP: {
            if (game)
                game->GetInputManager()->EmitMouseButtonUp(0);
        }
        break;

        case WM_RBUTTONDOWN: {
            if (game)
                game->GetInputManager()->EmitMouseButtonDown(1);
        }
        break;

        case WM_RBUTTONUP: {
            if (game)
                game->GetInputManager()->EmitMouseButtonUp(1);
        }
        break;

        case WM_MOUSEWHEEL: {
            const auto delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (game)
                game->GetInputManager()->EmitMouseScroll(delta);
        }
        break;

        case WM_SYSKEYDOWN:
            return OnSysKeyDown(lParam, wParam, windowState, game, hWnd);

        case WM_MENUCHAR:
            // A menu is active and the user presses a key that does not correspond
            // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
            return MAKELRESULT(0, MNC_CLOSE);

        case WM_SETCURSOR: {
            if (LOWORD(lParam) == HTCLIENT) {
                SetCursor(None);
                return true;
            }
        }
        break;

        default:
            break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

// Exit helper
void ExitGame() noexcept {
    PostQuitMessage(0);
}

LRESULT OnCreate(HWND hwnd, LPARAM lParam) {
    if (lParam) {
        const auto params = RCAST<LPCREATESTRUCTW>(lParam);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, RCAST<LONG_PTR>(params->lpCreateParams));
    }

    // Set window fullscreen
    {
        SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP);
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);

        SetWindowPos(hwnd,
                     HWND_TOP,
                     0,
                     0,
                     0,
                     0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

        ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    }

    return S_OK;
}

LRESULT OnPaint(HWND hwnd, Game* game, bool sizeMove) {
    if (sizeMove && game) {
        game->Tick();
    } else {
        PAINTSTRUCT ps;
        Ignore = BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
    }

    return S_OK;
}

LRESULT OnDisplayChange(Game* game) {
    if (game) {
        game->OnDisplayChange();
    }

    return S_OK;
}

LRESULT OnMove(Game* game) {
    if (game) {
        game->OnWindowMoved();
    }

    return S_OK;
}

LRESULT OnSize(Game* game, WindowState& windowState, LPARAM lParam, WPARAM wParam) {
    if (wParam == SIZE_MINIMIZED) {
        if (!windowState.Minimized) {
            windowState.Minimized = true;
            if (!windowState.Suspended && game)
                game->OnSuspending();
            windowState.Suspended = true;
        }
    } else if (windowState.Minimized) {
        windowState.Minimized = false;
        if (windowState.Suspended && game)
            game->OnResuming();
        windowState.Suspended = false;
    } else if (!windowState.SizeMove && game) {
        game->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
    }

    return S_OK;
}

LRESULT OnSizeMove(WindowState& windowState, bool isEnter, Game* game, HWND hwnd) {
    if (isEnter) {
        windowState.SizeMove = true;
        return S_OK;
    }

    windowState.SizeMove = false;
    if (game) {
        RECT rc;
        GetClientRect(hwnd, &rc);

        game->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
    }

    return S_OK;
}

LRESULT OnMinMaxInfo(LPARAM lParam) {
    if (lParam) {
        const auto info        = RCAST<MINMAXINFO*>(lParam);
        info->ptMinTrackSize.x = 640;
        info->ptMinTrackSize.y = 360;
    }

    return S_OK;
}

LRESULT OnActivateApp(Game* game, WPARAM wParam) {
    if (game) {
        if (wParam) {
            game->OnActivated();
        } else {
            game->OnDeactivated();
        }
    }

    return S_OK;
}

LRESULT OnPowerBroadcast(WindowState& windowState, Game* game, WPARAM wParam) {
    switch (wParam) {
        case PBT_APMQUERYSUSPEND:
            if (!windowState.Suspended && game)
                game->OnSuspending();
            windowState.Suspended = true;
            return TRUE;

        case PBT_APMRESUMESUSPEND:
            if (!windowState.Minimized) {
                if (windowState.Suspended && game)
                    game->OnResuming();
                windowState.Suspended = false;
            }
            return TRUE;
        default:
            break;
    }

    return S_OK;
}

LRESULT OnKeyDown(Game* game, WPARAM wParam) {
    // Escape key to quit
    if (wParam == VK_ESCAPE) {
        game->OnDeviceLost();
        ExitGame();
        return S_OK;
    }

    game->GetInputManager()->EmitKeyDown(CAST<u32>(wParam));

    return S_OK;
}

LRESULT OnSysKeyDown(LPARAM lParam,
                     WPARAM wParam,
                     WindowState& windowState,
                     Game* game,
                     HWND hwnd) {
    if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000) {
        // Implements the classic ALT+ENTER fullscreen toggle
        if (windowState.Fullscreen) {
            SetWindowLongPtr(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, 0);

            int width  = 1280;
            int height = 720;
            if (game)
                game->GetDefaultSize(width, height);

            ShowWindow(hwnd, SW_SHOWNORMAL);

            SetWindowPos(hwnd,
                         HWND_TOP,
                         0,
                         0,
                         width,
                         height,
                         SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
        } else {
            SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP);
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);

            SetWindowPos(hwnd,
                         HWND_TOP,
                         0,
                         0,
                         0,
                         0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

            ShowWindow(hwnd, SW_SHOWMAXIMIZED);
        }

        windowState.Fullscreen = !windowState.Fullscreen;
    }

    return S_OK;
}