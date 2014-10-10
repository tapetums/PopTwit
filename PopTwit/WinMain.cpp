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
// CRT を使わないため new/delete を自前で実装
//---------------------------------------------------------------------------//

#ifdef _NODEFLIB

void* __cdecl operator new(size_t size)
{
    return ::HeapAlloc(::GetProcessHeap(), 0, size);
}

void __cdecl operator delete(void* p)
{
    if ( p != nullptr ) ::HeapFree(::GetProcessHeap(), 0, p);
}

void* __cdecl operator new[](size_t size)
{
    return ::HeapAlloc(::GetProcessHeap(), 0, size);
}

void __cdecl operator delete[](void* p)
{
    if ( p != nullptr ) ::HeapFree(::GetProcessHeap(), 0, p);
}

#endif

//---------------------------------------------------------------------------//
// アプリケーション エントリポイント
//---------------------------------------------------------------------------//

#ifdef _NODEFLIB

// プログラムサイズを小さくするためにCRTを除外
#pragma comment(linker, "/nodefaultlib:libcmt.lib")
#ifdef _UNICODE
#pragma comment(linker, "/entry:wWinMain")
#else
#pragma comment(linker, "/entry:WinMain")
#endif

#endif

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

    // INI ファイルの書き込み
    SaveIniFile(ininame);

    // COM の終了処理
    ::CoUninitialize();

    return static_cast<INT32>(msg.wParam);
}

//---------------------------------------------------------------------------//

// WinMain.cpp