//---------------------------------------------------------------------------//
//
// WinMain.cpp
//  アプリケーション エントリポイント
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

// メモリリーク検出
#if defined(_DEBUG) || (DEBUG)
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
    #define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//---------------------------------------------------------------------------//

#include <windows.h>
#include <tchar.h>

#include "curl/curl.h"
#include "IniFile.h"
#include "Wnd.hpp"
#include "MainWindow.h"

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

    // インスタンスハンドルをグローバル変数に保存
    g_hInst = hInstance;

    // libcurl の初期化
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // INI ファイルの読み込み
    TCHAR ininame[MAX_PATH];
    const auto length = ::GetModuleFileName(nullptr, ininame, MAX_PATH);
    if ( length > 3 )
    {
        ininame[length - 3] = 'i';
        ininame[length - 2] = 'n';
        ininame[length - 1] = 'i';
    }
    const auto enabled = LoadIniFile(ininame);
    if ( !enabled )
    {
        return -2;
    }

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

    // libcurl の終了処理
    curl_global_cleanup();

    // INI ファイルの書き出し
    SaveIniFile(ininame);

    return static_cast<INT32>(msg.wParam);
}

//---------------------------------------------------------------------------//

// WinMain.cpp