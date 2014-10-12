//---------------------------------------------------------------------------//
//
// WinMain.cpp
//  アプリケーション エントリポイント
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>
#include <tchar.h>

#include "ConsoleOut.h"
#include "Wnd.hpp"
#include "MainWindow.h"

//---------------------------------------------------------------------------//

// メモリリーク検出
#if defined(_DEBUG) || (DEBUG)
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
    #define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//---------------------------------------------------------------------------//
// グローバル変数の実体定義
//---------------------------------------------------------------------------//

LPCTSTR   APP_NAME = TEXT("PopTrend");
HINSTANCE g_hInst  = nullptr;

//---------------------------------------------------------------------------//
// アプリケーション エントリポイント
//---------------------------------------------------------------------------//

INT32 WINAPI _tWinMain
(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    INT32     nCmdShow
)
{
    // メモリリーク検出
    #if defined(_DEBUG) || (DEBUG)
    {
        ::_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
    }
    #endif

    // 二重起動の防止
    const auto hMutex = ::CreateMutex(nullptr, FALSE, APP_NAME);
    if ( nullptr == hMutex )
    {
        //console_out(TEXT("%s: CreateMutex() failed"), APP_NAME);
        return -4;
    }
    else if ( ::GetLastError() == ERROR_ALREADY_EXISTS )
    {
        //console_out(TEXT("%s is already running"), APP_NAME);
        ::CloseHandle(hMutex);
        return -3;
    }

    // インスタンスハンドルをグローバル変数に保存
    g_hInst = hInstance;

    console_out(TEXT("----------------------------------------"));

    // ウィンドウの生成
    Wnd::Register(APP_NAME, MainWindowProc, MAKEINTRESOURCE(100));
    const auto hwnd = Wnd::Create
    (
        APP_NAME, APP_NAME, WS_SYSMENU | WS_THICKFRAME, 0
    );
    if ( hwnd == nullptr )
    {
        return -1;
    }
    Wnd::Resize(hwnd, 240, 300);
    Wnd::ToCenter(hwnd);
    Wnd::Show(hwnd);

    // メッセージループ
    MSG msg = { };
    while ( ::GetMessage(&msg, nullptr, 0, 0) > 0 )
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    if ( hMutex )
    {
        ::CloseHandle(hMutex);
    }

    console_out(TEXT("----------------------------------------\r\n"));

    return static_cast<INT32>(msg.wParam);
}

//---------------------------------------------------------------------------//

// WinMain.cpp