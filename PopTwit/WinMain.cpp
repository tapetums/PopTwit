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

#include "Wnd.hpp"
#include "IniFile.h"
#include "MainWindow.h"

//---------------------------------------------------------------------------//
// グローバル変数の実体宣言
//---------------------------------------------------------------------------//

extern LPCTSTR   APP_NAME = TEXT("PopTwit");
extern HINSTANCE g_hInst  = nullptr;

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
        return -4;
    }
    else if ( ::GetLastError() == ERROR_ALREADY_EXISTS )
    {
        ::CloseHandle(hMutex);
        return -3;
    }

    // インスタンスハンドルをグローバル変数に保存
    g_hInst = hInstance;

    // COM の初期化
    ::CoInitialize(nullptr);

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
        APP_NAME, APP_NAME, 0
    );
    if ( hwnd == nullptr )
    {
        return -1;
    }
    Wnd::Resize(hwnd, 400, 128);
    Wnd::ToCenter(hwnd);
    Wnd::Show(hwnd, SW_SHOW);

    // メッセージループ
    MSG msg = { };
    while ( ::GetMessage(&msg, nullptr, 0, 0) > 0 )
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    // INI ファイルの書き出し
    SaveIniFile(ininame);

    // COM の終了処理
    ::CoUninitialize();

    // ミューテックスの解放
    if ( hMutex )
    {
        ::CloseHandle(hMutex);
    }

    return static_cast<INT32>(msg.wParam);
}

//---------------------------------------------------------------------------//

// WinMain.cpp