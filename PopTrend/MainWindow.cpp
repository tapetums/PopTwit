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

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include "curl/curl.h"
#include "ConsoleOut.h"
#include "Wnd.hpp"
#include "MainWindow.h"

#ifdef _WIN64
#pragma comment(lib, "lib\\x64\\libcurl.lib")
#else
#pragma comment(lib, "lib\\x86\\libcurl.lib")
#endif

//---------------------------------------------------------------------------//
// MBCS と UTF-8 を区別するための文字列型
//---------------------------------------------------------------------------//

typedef unsigned char char8_t;

namespace std
{
    typedef std::basic_string<char8_t> u8string;
}

//---------------------------------------------------------------------------//
// グローバル変数
//---------------------------------------------------------------------------//

extern LPCTSTR   APP_NAME;
extern HINSTANCE g_hInst;

//---------------------------------------------------------------------------//
// グローバル変数の実体宣言
//---------------------------------------------------------------------------//

HTHEME g_hTheme = nullptr;
HWND   btn = nullptr;

CURL* curl = nullptr;
std::u8string chunk;

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
        case WM_NCHITTEST:
        {
            return HTCAPTION;
        }
        case WM_COMMAND:
        {
            return OnCommand(hwnd, LOWORD(wp), HIWORD(wp));
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
// コールバック関数
//---------------------------------------------------------------------------//

size_t write_data
(
    char8_t* str, size_t size, size_t nmemb, std::u8string* stream
)
{
    console_out(TEXT("write_data("));

    const auto cared_bytes = size * nmemb;
    if ( cared_bytes == 0 )
    {
        return 0;
    }
    console_out(TEXT("write_data(): %d bytes"), cared_bytes);

    stream->append(str, cared_bytes);

    return cared_bytes;
}

//---------------------------------------------------------------------------//
// イベントハンドラ
//---------------------------------------------------------------------------//

LRESULT __stdcall OnCreate(HWND hwnd)
{
    btn = ::CreateWindowEx
    (
        0, TEXT("BUTTON"), TEXT("OK"),
        WS_CHILD | WS_VISIBLE |
        BS_PUSHBUTTON | BS_CENTER | BS_VCENTER,
        0, 0, 24, 24,
        hwnd, (HMENU)1001, g_hInst, nullptr
    );

    if ( curl == nullptr )
    {
        curl = curl_easy_init();
        console_out(TEXT("curl_easy_init(): %s"), curl ? TEXT("OK") : TEXT("FAIL"));
        if ( curl == nullptr )
        {
            return 0;
        }
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.twitter.com/1.1/trends/place.json");
    //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

    // Aero Glass 効果をオンに
    OnThemeChanged(hwnd);

    return 0;
}

//---------------------------------------------------------------------------//

LRESULT __stdcall OnDestroy(HWND hwnd)
{
    if ( curl )
    {
        console_out(TEXT("curl_easy_cleanup()"));
        curl_easy_cleanup(curl);
        curl = nullptr;
    }

    // アプリケーションを終了
    ::PostQuitMessage(0);

    return 0;
}

//---------------------------------------------------------------------------//

LRESULT __stdcall OnSize(HWND hwnd, INT32 w, INT32 h)
{
    Wnd::Bounds(btn, (w - 48) /2, h - 32, 48, 24);
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
        case 1001:
        {
            if ( curl == nullptr )
            {
                break;
            }

            const auto ret = curl_easy_perform(curl);
            console_out(TEXT("ret = %d"), ret);
            if ( ret != CURLE_OK )
            {
                console_out(TEXT("curl_easy_perform() failed"));
                break;
            }

            const auto hFile = ::CreateFile
            (
                TEXT("R:\\afo.json"), GENERIC_WRITE, FILE_SHARE_WRITE, nullptr,
                OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr
            );
            if ( hFile == nullptr || hFile == INVALID_HANDLE_VALUE )
            {
                console_out(TEXT("Not found"));
                break;
            }

            DWORD dw;
            const auto len = chunk.length();
            ::WriteFile(hFile, chunk.c_str(), len, &dw, nullptr);

            ::CloseHandle(hFile);
            console_out(TEXT("Wrote %d bytes"), dw);

            break;
        }
        default:
        {
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

// MainWindow.hpp