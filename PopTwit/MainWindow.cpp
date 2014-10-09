//---------------------------------------------------------------------------//
//
// MainWindow.cpp
//  メインウィンドウ
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>
#include <strsafe.h>

#undef DeleteFont

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include "shlobj.h"
#include <shobjidl.h>

#include "Wnd.hpp"
#include "Twitter.h"
#include "IniFile.h"
#include "MainWindow.h"
#include "MainWindow.Utl.hpp"

//---------------------------------------------------------------------------//
// 画面の大きさに関する定数
//---------------------------------------------------------------------------//

#define LEFTEXTENDWIDTH   8
#define RIGHTEXTENDWIDTH  8
#define BOTTOMEXTENDWIDTH 8
#define TOPEXTENDWIDTH    32

//---------------------------------------------------------------------------//
// グローバル変数
//---------------------------------------------------------------------------//

extern LPCTSTR   APP_NAME;
extern HINSTANCE g_hInst;

//---------------------------------------------------------------------------//
// グローバル変数の実体宣言
//---------------------------------------------------------------------------//

HTHEME  g_hTheme      = nullptr;
INT32   g_font_size   = 20;
LPCTSTR g_font_tweet  = TEXT("Meiryo");;
LPCTSTR g_font_symbol = TEXT("Segoe UI Symbol");;

HFONT font_tweet, font_button, font_symbol;
HWND  cbx_account, txt_tweet;
HWND  btn_send, btn_close, btn_shorten, btn_pic, btn_new_account;

//---------------------------------------------------------------------------//
// コントロール識別子
//---------------------------------------------------------------------------//

enum Controls : UINT32
{
    CTRL_CBX_ACCOUNT = 40001,
    CTRL_TXT_TWEET,
    CTRL_BTN_SEND,
    CTRL_BTN_CLOSE,
    CTRL_BTN_PICTURE,
    CTRL_BTN_SHORTEN_URL,
};

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
        case WM_NCCALCSIZE:
        {
            if ( wp ) { return 0; }
            else      {  break; }
        }
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
        case WM_SETFOCUS:
        {
            ::SetFocus(txt_tweet); return 0;
        }
        case WM_PAINT:
        {
            return OnPaint(hwnd);
        }
        case WM_NCHITTEST:
        {
            return (LRESULT)HTCAPTION;
        }
        case WM_KEYDOWN:
        {
            return OnKeyDown(hwnd, (INT16)wp, (INT16)lp);
        }
        case WM_COMMAND:
        {
            return OnCommand(hwnd, LOWORD(wp));
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
    font_tweet  = MakeFont(g_font_size, g_font_tweet);
    font_button = MakeFont(16, g_font_symbol);
    font_symbol = MakeFont(20, g_font_symbol);

    cbx_account =::CreateWindowEx
    (
        WS_EX_ACCEPTFILES, TEXT("COMBOBOX"), TEXT("アカウントを選択"),
        WS_CHILD | WS_BORDER | WS_VISIBLE |
        CBS_DROPDOWNLIST,
        0, 0, 128, 24,
        hwnd, nullptr, g_hInst, nullptr
    );

    ::SendMessage(cbx_account, CB_SETCURSEL, 0, 0);
    for ( size_t index = 0; index < MAX_ACCOUNT; ++index )
    {
        const auto username = g_username[index];
        if ( username[0] == '\0' )
        {
            break;
        }
        ::SendMessage(cbx_account, CB_ADDSTRING, index, (LPARAM)username);

        if ( index == g_user_index )
        {
            ::SendMessage(cbx_account, CB_SETCURSEL, index, 0);
        }
    }
    ::SendMessage(cbx_account, WM_SETFONT, (WPARAM)font_tweet, (LPARAM)FALSE);
    ::SetWindowSubclass
    (
        cbx_account, (SUBCLASSPROC)ComboBoxProc, CTRL_CBX_ACCOUNT, (DWORD_PTR)hwnd
    );

    txt_tweet = ::CreateWindowEx
    (
        WS_EX_ACCEPTFILES, TEXT("EDIT"), TEXT(""),
        WS_CHILD | WS_BORDER | WS_VISIBLE |
        ES_LEFT | ES_NOHIDESEL | ES_MULTILINE | ES_AUTOVSCROLL |
        ES_WANTRETURN,
        0, 0, 320, 240,
        hwnd, nullptr, g_hInst, nullptr
    );
    ClearMessagestub(g_msgstub);
    LoadMessageStub(g_msgstub);
    ::SendMessage(txt_tweet, WM_SETFONT, (WPARAM)font_tweet, (LPARAM)FALSE);
    ::SendMessage(txt_tweet, WM_SETTEXT, 0, (LPARAM)g_msgstub);
    ::SetWindowSubclass
    (
        txt_tweet, (SUBCLASSPROC)TextBoxProc, CTRL_TXT_TWEET, (DWORD_PTR)hwnd
    );

    btn_send = ::CreateWindowEx
    (
        0, TEXT("BUTTON"), TEXT("📤 Tweet"),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_CENTER | BS_VCENTER,
        0, 0, 48, 24,
        hwnd, nullptr, g_hInst, nullptr
    );
    ::SendMessage(btn_send, WM_SETFONT, (WPARAM)font_button, (LPARAM)FALSE);
    ::SetWindowSubclass
    (
        btn_send, (SUBCLASSPROC)ButtonProc, CTRL_BTN_SEND, (DWORD_PTR)hwnd
    );

    btn_close = ::CreateWindowEx
    (
        0, TEXT("BUTTON"), TEXT("⛝ 閉じる"),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_CENTER | BS_VCENTER,
        0, 0, 48, 24,
        hwnd, nullptr, g_hInst, nullptr
    );
    ::SendMessage(btn_close, WM_SETFONT, (WPARAM)font_button, (LPARAM)FALSE);
    ::SetWindowSubclass
    (
        btn_close, (SUBCLASSPROC)ButtonProc, CTRL_BTN_CLOSE, (DWORD_PTR)hwnd
    );

    btn_shorten = ::CreateWindowEx
    (
        0, TEXT("BUTTON"), TEXT("🔗"),
        WS_CHILD | WS_VISIBLE | WS_DISABLED |
        BS_PUSHBUTTON | BS_CENTER | BS_VCENTER,
        0, 0, 24, 24,
        hwnd, nullptr, g_hInst, nullptr
    );
    ::SendMessage(btn_shorten, WM_SETFONT, (WPARAM)font_symbol, (LPARAM)FALSE);
    ::SetWindowSubclass
    (
        btn_shorten, (SUBCLASSPROC)ButtonProc, CTRL_BTN_SHORTEN_URL, (DWORD_PTR)hwnd
    );

    btn_pic = ::CreateWindowEx
    (
        0, TEXT("BUTTON"), TEXT("📷"),
        WS_CHILD | WS_VISIBLE | WS_DISABLED |
        BS_PUSHBUTTON | BS_CENTER | BS_VCENTER,
        0, 0, 24, 24,
        hwnd, nullptr, g_hInst, nullptr
    );
    ::SendMessage(btn_pic, WM_SETFONT, (WPARAM)font_symbol, (LPARAM)FALSE);
    ::SetWindowSubclass
    (
        btn_pic, (SUBCLASSPROC)ButtonProc, CTRL_BTN_PICTURE, (DWORD_PTR)hwnd
    );

    ::BufferedPaintInit();
    OnThemeChanged(hwnd);

    ::SetFocus(txt_tweet);

    return 0;
}

//---------------------------------------------------------------------------//

LRESULT __stdcall OnDestroy(HWND hwnd)
{
    g_user_index = ::SendMessage(cbx_account, CB_GETCURSEL, 0, 0);
    ::GetWindowText(txt_tweet, g_msgstub, MAX_MESSAGE_LEN);
    SaveMessageStub(g_msgstub);

    DeleteFont(font_tweet);
    DeleteFont(font_button);

    if ( g_hTheme )
    {
        ::CloseThemeData(g_hTheme);
        g_hTheme = nullptr;
    }

    ::BufferedPaintUnInit();
    ::PostQuitMessage(0);

    return 0;
}

//---------------------------------------------------------------------------//

LRESULT __stdcall OnSize(HWND hwnd, INT32 w, INT32 h)
{
    static const auto EXWIDTH = LEFTEXTENDWIDTH + RIGHTEXTENDWIDTH;

    Wnd::Bounds
    (
        cbx_account,
        w - 128 - LEFTEXTENDWIDTH, 5, 128, 24
    );
    Wnd::Bounds
    (
        txt_tweet,
        LEFTEXTENDWIDTH, 4 + TOPEXTENDWIDTH, w - EXWIDTH, h - 40 - TOPEXTENDWIDTH
    );
    Wnd::Bounds
    (
        btn_send,
        w - 64 - RIGHTEXTENDWIDTH, h - 24 - BOTTOMEXTENDWIDTH, 64, 24
    );
    Wnd::Bounds
    (
        btn_close,
        w - 132 - RIGHTEXTENDWIDTH, h - 24 - BOTTOMEXTENDWIDTH, 64, 24
    );
    Wnd::Bounds
    (
        btn_shorten,
        4 + 24 + LEFTEXTENDWIDTH, h - 24 - BOTTOMEXTENDWIDTH, 24, 24
    );
    Wnd::Bounds
    (
        btn_pic,
        LEFTEXTENDWIDTH, h - 24 - BOTTOMEXTENDWIDTH, 24, 24
    );

    return 0;
}

//---------------------------------------------------------------------------//

LRESULT __stdcall OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    const auto hDC = BeginPaint(hwnd, &ps);

    if ( Wnd::IsCompositionEnabled() )
    {
        ::FillRect(hDC, &ps.rcPaint, (HBRUSH)::GetStockObject(BLACK_BRUSH));
    }
    else
    {
        ::FillRect(hDC, &ps.rcPaint, (HBRUSH)::GetStockObject(WHITE_BRUSH));
    }

    EndPaint(hwnd, &ps);

    return 0;
}

//---------------------------------------------------------------------------//

LRESULT __stdcall OnKeyDown(HWND hwnd, INT16 nVirtKey, INT16 lKeyData)
{
    switch ( nVirtKey )
    {
        case VK_RETURN:
        {
            if ( 0x80 & ::GetKeyState(VK_CONTROL) )
            {
                OnCommand(hwnd, CTRL_BTN_SEND);
            }
            break;
        }
        case VK_ESCAPE:
        {
            OnCommand(hwnd, CTRL_BTN_CLOSE);
            break;
        }
        case VK_UP:
        case VK_DOWN:
        {
            if ( 0x80 & ::GetKeyState(VK_CONTROL) )
            {
                const auto count = ::SendMessage(cbx_account, CB_GETCOUNT, 0, 0);
                auto index = ::SendMessage(cbx_account, CB_GETCURSEL, 0, 0);

                if ( nVirtKey == VK_UP && index > 0 && --index < count )
                {
                    ::SendMessage(cbx_account, CB_SETCURSEL, index, 0);
                }
                if ( nVirtKey == VK_DOWN && index <count && ++index >= 0 )
                {
                    ::SendMessage(cbx_account, CB_SETCURSEL, index, 0);
                }
                Wnd::Refresh(cbx_account);
            }
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

LRESULT __stdcall OnCommand(HWND hwnd, UINT16 wId)
{
    switch ( wId )
    {
        case CTRL_BTN_SEND:
        {
            const auto index = ::SendMessage
            (
                cbx_account, CB_GETCURSEL, 0, 0
            );
            if ( index == 0 )
            {
                ::MessageBox
                (
                    nullptr, TEXT("アカウントが選択されていません"), APP_NAME, MB_OK
                );
                ::SetFocus(txt_tweet);
                break;
            }

            TCHAR username[MAX_PATH];
            ::SendMessage
            (
                cbx_account, CB_GETLBTEXT, index, (LPARAM)username
            );

            TCHAR message[MAX_PATH];
            ::GetWindowText(txt_tweet, message, MAX_PATH);

            const auto ret = Tweet(hwnd, index, username, message);
            if ( ret )
            {
                ClearMessagestub(g_msgstub);
                ::SendMessage(txt_tweet, WM_SETTEXT, 0, (LPARAM)g_msgstub);
            }
            ::SetFocus(txt_tweet);

            break;
        }
        case CTRL_BTN_CLOSE:
        {
            ::PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        }
        case CTRL_BTN_PICTURE:
        {
            ::MessageBox(nullptr, TEXT("未実装です"), APP_NAME, MB_OK);
            ::SetFocus(txt_tweet);
            break; // TODO: UploadPicture() の実装
        }
        case CTRL_BTN_SHORTEN_URL:
        {
            ::MessageBox(nullptr, TEXT("未実装です"), APP_NAME, MB_OK);
            ::SetFocus(txt_tweet);
            break; // TODO: ShortenURL() の実装
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