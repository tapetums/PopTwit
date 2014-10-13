//---------------------------------------------------------------------------//
//
// MainWindow.Utl.hpp
//  ユーティリティ関数
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

#pragma once

#include <vsstyle.h>
#include <vssym32.h>

#include "picojson.h"

#include "../Transcode.hpp"

//---------------------------------------------------------------------------//
// グローバル変数
//---------------------------------------------------------------------------//

extern LPCTSTR APP_NAME;
extern HTHEME g_hTheme;

// フォント情報
extern HFONT font_label, font_info, font_symbol;

// 子コントロールと関連付ける文字列のバッファ
extern TCHAR info_txt[MAX_PATH];
extern TCHAR trend_text[10][MAX_PATH];
extern TCHAR trend_link[10][MAX_PATH];

//---------------------------------------------------------------------------//
// 内部関数
//---------------------------------------------------------------------------//

bool __stdcall ParseJson(const std::string& reply)
{
    // picojson で パースする
    std::string err;
    picojson::value value;
    picojson::parse(value, reply.begin(), reply.end(), &err);
    if ( !err.empty() )
    {
        console_outA("json parse error: %s", err.c_str());
        return false;
    }
    else if ( ! value.is<picojson::array>() )
    {
        console_outA("Unparsable json format ... the first object is not an array");
        return false;
    }

    // 要素を取り出す
    auto& json = value.get<picojson::array>()[0].get<picojson::object>();
    auto& trends     = json["trends"].get<picojson::array>();
    auto  as_of      = json["as_of"].get<std::string>().c_str();
    auto  at         = json["created_at"].get<std::string>().c_str();
    auto& locations  = json["locations"].get<picojson::array>();

    const char year[5]   = { at[0], at[1], at[2], at[3], '\0' };
    const char month[3]  = { at[5], at[6], '\0' };
    const char day[3]    = { at[8], at[9], '\0' };
    const char hour[3]   = { at[11], at[12], '\0' };
    const char minute[3] = { at[14], at[15], '\0' };
    const char second[3] = { at[17], at[18], '\0' };

    // 取得日時をローカル日時に変換して再度文字列に
    SYSTEMTIME st =
    {
        std::stoi(year), std::stoi(month), 0, std::stoi(day),
        std::stoi(hour), std::stoi(minute), std::stoi(second), 0
    };
    FILETIME ft, ft_local;
    ::SystemTimeToFileTime(&st,&ft);
    ::FileTimeToLocalFileTime(&ft, &ft_local);
    ::FileTimeToSystemTime(&ft_local, &st);
    ::StringCchPrintf
    (
        info_txt, MAX_PATH, TEXT("@ %04d/%02d/%02d %02d:%02d:%02d"),
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond
    );

    // トレンドのテキストとリンクを取り出す
    size_t index = 0;
    for ( auto& trend: trends )
    {
        if ( ! trend.is<picojson::object>() )
        {
            break;
        }
        const auto& name = trend.get<picojson::object>()["name"].get<std::string>();
        const auto& url = trend.get<picojson::object>()["url"].get<std::string>();

#if defined _UNICODE || UNICODE
        const auto text = (LPCWSTR)toUTF16((char8_t*)name.c_str());
#else
        const auto text = (LPCSTR)toMBCS((char8_t*)name.c_str());
#endif
        ::StringCchCopy(trend_text[index], MAX_PATH, text);

#if defined _UNICODE || UNICODE
        const auto link = (LPCWSTR)toUTF16((char8_t*)url.c_str());
#else
        const auto link = (LPCSTR)toMBCS((char8_t*)url.c_str());
#endif
        ::StringCchCopy(trend_link[index], MAX_PATH, link);

        ++index;
    }

    return true;
}

//---------------------------------------------------------------------------//

void __stdcall JumpToTrendPage(size_t label_index)
{
    if ( trend_link[label_index][0] == '\0' )
    {
        return;
    }
    ::ShellExecute
    (
        nullptr, TEXT("open"), trend_link[label_index],
        nullptr, nullptr, SW_SHOW
    );
}

//---------------------------------------------------------------------------//
// サブクラス プロシージャ
//---------------------------------------------------------------------------//

LRESULT CALLBACK LabelProc
(
    HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData
)
{
    if ( uMsg != WM_PAINT )
    {
        return ::DefSubclassProc(hwnd, uMsg, wp, lp);
    }

    // DrawThemeTextEx() のためのオプション
    static const DTTOPTS dttOpts =
    {
        sizeof(DTTOPTS),
        DTT_GLOWSIZE | DTT_COMPOSITED,
        (COLORREF)0, (COLORREF)0, (COLORREF)0,
        TST_SINGLE,
        { 2, 1 },
        0, 0, 0,
        0,
        TRUE,
        8,
        nullptr, (LPARAM)0
    };

    PAINTSTRUCT ps;
    const auto hDC = ::BeginPaint(hwnd, &ps);
    const auto prc = &ps.rcPaint;

    HDC hMemDC = nullptr;
    const auto pb = ::BeginBufferedPaint
    (
        hDC, prc, BPBF_TOPDOWNDIB, 0, &hMemDC
    );
    if ( pb )
    {
        LPWSTR text = nullptr;

        if ( (UINT16)uIdSubclass == CTRL_LABAEL_INFO )
        {
            // 背景をクリア
            HBRUSH hbr = nullptr;
            if ( Wnd::IsCompositionEnabled() )
            {
                hbr = (HBRUSH)::GetStockObject(BLACK_BRUSH);
            }
            else
            {
                hbr = (HBRUSH)::GetStockObject(COLOR_BTNFACE);
            }
            ::FillRect(hMemDC, prc, hbr);

            // 描画すべき文字列を取得
        #if defined _UNICODE || UNICODE
            text = info_txt;
        #else
            text = (LPWSTR)toUTF16((char*)info_txt);
        #endif

            // フォントをセット
            ::SelectObject(hMemDC, font_info);
        }
        else
        {
            // 背景をクリア (見やすくなるよう薄く色を付ける)
            HBRUSH hbr = nullptr;
            if ( Wnd::IsCompositionEnabled() )
            {
                hbr = (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
            }
            else
            {
                hbr = (HBRUSH)::GetStockObject(COLOR_BTNFACE);
            }
            ::FillRect(hMemDC, prc, hbr);
            ::BufferedPaintSetAlpha(pb, prc, 128);

            // 描画すべき文字列を取得
            const auto index = (UINT16)uIdSubclass - CTRL_LABEL_TREND;
#if defined _UNICODE || UNICODE
            text = trend_text[index];
#else
            text = (LPWSTR)toUTF16((char*)trend_text[index]);
#endif

            // フォントをセット
            ::SelectObject(hMemDC, font_label);
        }

        // 文字列を描画
        prc->top += 4;
        ::DrawThemeTextEx
        (
            g_hTheme, hMemDC, TEXT_CONTROLLABEL, 0,
            text, lstrlenW(text),
            DT_CENTER | DT_VCENTER | DT_MODIFYSTRING,
            prc, &dttOpts
        );
        ::EndBufferedPaint(pb, TRUE);
    }

    ::EndPaint(hwnd, &ps);

    return 0;
};

//---------------------------------------------------------------------------//

LRESULT CALLBACK ButtonProc
(
    HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData
)
{
    if ( uMsg == WM_LBUTTONUP )
    {
        OnCommand((HWND)dwRefData, (UINT16)uIdSubclass, 0);
    }
    if ( uMsg != WM_PAINT )
    {
        return ::DefSubclassProc(hwnd, uMsg, wp, lp);
    }

    PAINTSTRUCT ps;
    const auto hDC = ::BeginPaint(hwnd, &ps);
    const auto prc = &ps.rcPaint;

    HDC hMemDC = nullptr;
    const auto pb = ::BeginBufferedPaint
    (
        hDC, prc, BPBF_TOPDOWNDIB, 0, &hMemDC
    );
    if ( pb )
    {
        ::SendMessage(hwnd, WM_PRINTCLIENT, (WPARAM)hMemDC, PRF_CLIENT);
        ::BufferedPaintSetAlpha(pb, prc, 255);
        ::EndBufferedPaint(pb, TRUE);
    }

    ::EndPaint(hwnd, &ps);

    return 0;
};

//---------------------------------------------------------------------------//

// MainWindow.Utl.hpp