#include "pch.h"
#include "GomokuDefines.h"
#include "App.h"
#include "Gomoku.h"

using namespace gomoku;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) == SOCKET_ERROR)
    {
        return 1;
    }

    ghInst = hInstance;

    WNDCLASSEXW wcex;
    memset(&wcex, 0, sizeof(WNDCLASSEXW));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = App::WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GOMOKU));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"Gomoku";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    RegisterClassExW(&wcex);

    HWND hWnd = CreateWindowW(
        L"Gomoku",
        L"Gomoku",
        WS_OVERLAPPEDWINDOW,
        WINDOW_START_X, WINDOW_START_Y,
        RESOLUTION, RESOLUTION,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hWnd)
    {
        goto WSA_CLEAN;
    }

    if (FAILED(App::Init(hWnd)))
    {
        goto APP_RELEASE;
    }

    ShowWindow(hWnd, nCmdShow);
    App::Run();

APP_RELEASE:
    App::Release();

WSA_CLEAN:
    WSACleanup();
    return 0;
}