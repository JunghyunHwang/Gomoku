#include "pch.h"
#include "Gomoku.h"
#include "GameManager.h"
#include "App.h"

using namespace gomoku;

gomoku::App* InitInstance(HINSTANCE, int);

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

    GameManager* mInstance = GameManager::GetInstance();
    int iResult;
    SOCKADDR_IN oppnentAddr = { 0, };

    iResult = mInstance->Bind();
    if (iResult != S_OK)
    {
        std::cout << "ERROR: Failed to Bind with error code: " << iResult << std::endl;
    }

    iResult = mInstance->FindOppnent(oppnentAddr);
    if (iResult != S_OK)
    {
        std::cout << "ERROR: Failed to connect server with error code: " << iResult << std::endl;
    }

    iResult = mInstance->ConnectOppnent(oppnentAddr);
    if (iResult != S_OK)
    {
        std::cout << "ERROR: Failed to connect oppnent with error code: " << iResult << std::endl;
    }
    
    /*WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = App::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GOMOKU));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"Gomoku";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    RegisterClassExW(&wcex);

    App* app = InitInstance(hInstance, nCmdShow);
    {
        if (app == nullptr)
        {
            return FALSE;
        }

        app->Run();
    }
    app->Release();*/

    WSACleanup();
    return 0;
}

gomoku::App* InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd = CreateWindowW(
       L"Gomoku",
       L"Gomoku",
       WS_OVERLAPPEDWINDOW,
       500, 200, 800, 800,
       nullptr, nullptr, hInstance, nullptr
   );

   if (!hWnd)
   {
      return FALSE;
   }

   App* app = gomoku::App::GetInstance();

   if (FAILED(app->Init(hInstance, hWnd, POINT{ RESOLUTION, RESOLUTION })))
   {
       app->Release();
       return nullptr;
   }

   ShowWindow(hWnd, nCmdShow);

   return app;
}