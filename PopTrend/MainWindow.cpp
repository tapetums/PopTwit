//---------------------------------------------------------------------------//
//
// MainWindow.cpp
//  メインウィンドウ
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

#include <string>

#include <windows.h>
#include <strsafe.h>

#ifdef _NODEFLIB
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#undef StringCchPrintf
#define StringCchPrintf wnsprintf
#endif

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include "../Font.hpp"
#include "ConsoleOut.h"
#include "IniFile.h"
#include "Wnd.hpp"
#include "Twitter.h"
#include "MainWindow.h"
#include "MainWindow.Utl.hpp"

//---------------------------------------------------------------------------//
// グローバル変数
//---------------------------------------------------------------------------//

extern LPCTSTR   APP_NAME;
extern HINSTANCE g_hInst;

//---------------------------------------------------------------------------//
// グローバル変数の実体宣言
//---------------------------------------------------------------------------//

// ビジュアルスタイル テーマ
HTHEME g_hTheme = nullptr;

// フォント情報
LPCTSTR g_font_label  = TEXT("Meiryo");;
LPCTSTR g_font_symbol = TEXT("Segoe UI Symbol");;
HFONT font_label, font_info, font_symbol;

// 子コントロールのハンドル
HWND btn_update;
HWND label_info;
HWND label_trend[10];

// 子コントロールと関連付ける文字列のバッファ
TCHAR info_txt[MAX_PATH];
TCHAR trend_text[10][MAX_PATH];
TCHAR trend_link[10][MAX_PATH];

//---------------------------------------------------------------------------//
// ウィンドウプロシージャ
//---------------------------------------------------------------------------//

LRESULT __stdcall MainWindowProc
(
    HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp
)
{
    switch ( uMsg )
    {
        case WM_CREATE:
        {
            return OnCreate(hwnd);
        }
        case WM_DESTROY:
        {
            return OnDestroy(hwnd);
        }
        case WM_SIZE:
        {
            return OnSize(hwnd, LOWORD(lp), HIWORD(lp));
        }
        case WM_ERASEBKGND:
        {
            return OnEraseBkGnd((HDC)wp);
        }
        case WM_COMMAND:
        {
            return OnCommand(hwnd, LOWORD(wp), HIWORD(wp));
        }
        case WM_TIMER:
        {
            return OnCommand(hwnd, CTRL_BTN_UPDATE, 0);
        }
        case WM_DWMCOMPOSITIONCHANGED:
        {
            return OnDwmCompositionChanged(hwnd);
        }
        case WM_THEMECHANGED:
        {
            return OnThemeChanged(hwnd);
        }
        default:
        {
            break;
        }
    }

    return ::DefWindowProc(hwnd, uMsg, wp, lp);
}

//---------------------------------------------------------------------------//
// イベントハンドラ
//---------------------------------------------------------------------------//

LRESULT __stdcall OnCreate(HWND hwnd)
{
    // ダブルバッファリングを開始
    ::BufferedPaintInit();

    // フォントを生成
    font_label  = MakeFont(20, g_font_label);
    font_info   = MakeFont(18, g_font_label);
    font_symbol = MakeFont(20, g_font_symbol);

    // 子コントロールを生成
    btn_update = ::CreateWindowEx
    (
#if defined _UNICODE || UNICODE
        0, TEXT("BUTTON"), TEXT("📡"),
#else
        0, TEXT("BUTTON"), TEXT("更新"),
#endif
        WS_CHILD | WS_VISIBLE |
        BS_PUSHBUTTON | BS_CENTER | BS_VCENTER,
        0, 0, 24, 24,
        hwnd, (HMENU)CTRL_BTN_UPDATE, g_hInst, nullptr
    );
    ::SendMessage(btn_update, WM_SETFONT, (WPARAM)font_symbol, (LPARAM)FALSE);
    ::SetWindowSubclass
    (
        btn_update, (SUBCLASSPROC)ButtonProc, CTRL_BTN_UPDATE, (DWORD_PTR)hwnd
    );

    label_info = ::CreateWindowEx
    (
        0, TEXT("STATIC"), TEXT(""),
        WS_CHILD | WS_VISIBLE |
        SS_LEFT | SS_NOTIFY,
        0, 0, 24, 24,
        hwnd, (HMENU)CTRL_LABAEL_INFO, g_hInst, nullptr
    );
    ::SendMessage(label_info, WM_SETFONT, (WPARAM)font_info, (LPARAM)FALSE);
    ::SetWindowText(label_info, TEXT("なう"));
    ::SetWindowSubclass
    (
        label_info, (SUBCLASSPROC)LabelProc, CTRL_LABAEL_INFO, (DWORD_PTR)hwnd
    );

    for ( size_t index = 0; index < 10; ++index )
    {
        trend_link[index][0] = '\0';

        label_trend[index] = ::CreateWindowEx
        (
            0, TEXT("STATIC"), TEXT(""),
            WS_CHILD | WS_VISIBLE |
            SS_CENTER | SS_NOTIFY,
            0, 0, 24, 24,
            hwnd, (HMENU)(CTRL_LABEL_TREND + index), g_hInst, (LPVOID)trend_link[index]
        );
        ::SendMessage(label_trend[index], WM_SETFONT, (WPARAM)font_label, (LPARAM)FALSE);
        ::SetWindowText(label_trend[index], TEXT("-"));
        ::SetWindowSubclass
        (
            label_trend[index], (SUBCLASSPROC)LabelProc,
            CTRL_LABEL_TREND + index, (DWORD_PTR)hwnd
        );
    }

    // Aero Glass 効果をオンに
    OnThemeChanged(hwnd);

    // 情報取得
    OnCommand(hwnd, CTRL_BTN_UPDATE, 0);

    return 0;
}

//---------------------------------------------------------------------------//

LRESULT __stdcall OnDestroy(HWND hwnd)
{
    // タイマーを破棄
    ::KillTimer(hwnd, 2000);

    // フォントを破棄
    DeleteFont(font_label);
    DeleteFont(font_info);
    DeleteFont(font_symbol);

    // ダブルバッファリングを終了
    ::BufferedPaintUnInit();

    // アプリケーションを終了
    ::PostQuitMessage(0);

    return 0;
}

//---------------------------------------------------------------------------//

LRESULT __stdcall OnSize(HWND hwnd, INT32 w, INT32 h)
{
    Wnd::Bounds(btn_update, w - 48, h - 25, 48, 24);
    Wnd::Bounds(label_info, 0, h - 25, w - 52, 24);

    for ( INT32 index = 0; index < 10; ++index )
    {
        Wnd::Bounds(label_trend[index], 0, index * 27, w, 27);
    }

    return 0;
}

//---------------------------------------------------------------------------//

LRESULT __stdcall OnEraseBkGnd(HDC hDC)
{
    RECT rc;
    ::GetClipBox(hDC, &rc);

    HBRUSH hbr;
    if ( Wnd::IsCompositionEnabled() )
    {
        hbr = (HBRUSH)GetStockObject(BLACK_BRUSH);
    }
    else
    {
        hbr = GetSysColorBrush(COLOR_BTNFACE);
    }
    ::FillRect(hDC, &rc, hbr);

    return 0;
}

//---------------------------------------------------------------------------//

LRESULT __stdcall OnCommand(HWND hwnd, UINT16 wId, UINT16 nCode)
{
    switch ( wId )
    {
        case CTRL_BTN_UPDATE:
        {
            // トレンドを取得
            const auto reply = GetTrends
            (
                hwnd, g_username, g_woeid, g_no_hashtags
            );
            if ( reply.empty() )
            {
                // タイマーを破棄
                ::KillTimer(hwnd, 2000);
                break;
            }

            // 文字列を取り出してグローバル変数に格納
            if ( ! ParseJson(reply) )
            {
                break;
            }
            // タイマーを設定
            if ( g_interval > 0 )
            {
                ::SetTimer(hwnd, 2000, g_interval * 60 * 1000, nullptr);
            }

            // 描画を更新
            Wnd::Refresh(hwnd);

            break;
        }
        default:
        {
            if ( CTRL_LABEL_TREND <= wId && wId < CTRL_LABEL_TREND + 10 )
            {
                // ウェブページを開く
                JumpToTrendPage(wId - CTRL_LABEL_TREND);
            }
            break;
        }
    }

    return 0;
}

//---------------------------------------------------------------------------//

LRESULT __stdcall OnDwmCompositionChanged(HWND hwnd)
{
    if ( Wnd::IsCompositionEnabled() )
    {
        Wnd::EnableAero(hwnd);
    }
    else
    {
        Wnd::DisableAero(hwnd);
    }

    Wnd::Refresh(hwnd);

    return 0;
}

//---------------------------------------------------------------------------//

LRESULT __stdcall OnThemeChanged(HWND hwnd)
{
    if ( g_hTheme )
    {
        ::CloseThemeData(g_hTheme);
        g_hTheme = nullptr;
    }
    if ( ::IsThemeActive() )
    {
        g_hTheme = ::OpenThemeData(hwnd, L"BUTTON");
    }

    return ::OnDwmCompositionChanged(hwnd);
}

//---------------------------------------------------------------------------//

// MainWindow.cpp