//---------------------------------------------------------------------------//
//
// IniFile.cpp
//  INI 設定ファイル 操作関数
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>
#include <strsafe.h>

#ifdef _NODEFLIB
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#undef StringCchPrintf
#define StringCchPrintf wnsprintf
#endif

#include "IniFile.h"

//---------------------------------------------------------------------------//
// グローバル変数の実体宣言
//---------------------------------------------------------------------------//

UINT32 g_woeid;
bool   g_no_hashtags;
UINT32 g_interval;
TCHAR  g_username[MAX_PATH];

//---------------------------------------------------------------------------//

// INI ファイルの読み込み
bool __stdcall LoadIniFile(LPCTSTR ininame)
{
    const auto enabled = (bool)::GetPrivateProfileInt
    (
        TEXT("settings"), TEXT("enabled"), (INT)true, ininame
    );
    if ( !enabled )
    {
        return false;
    }

    g_woeid = ::GetPrivateProfileInt
    (
        TEXT("settings"), TEXT("woeid"), 23424856/* WOEID of Japan */, ininame
    );

    g_no_hashtags = (bool)::GetPrivateProfileInt
    (
        TEXT("settings"), TEXT("no_hashtags"), (INT)false, ininame
    );

    g_interval = ::GetPrivateProfileInt
    (
        TEXT("settings"), TEXT("interval"), 0, ininame
    );

    ::GetPrivateProfileString
    (
        TEXT("settings"), TEXT("user"),
        TEXT(""), g_username, MAX_PATH, ininame
    );

    return true;
}

//---------------------------------------------------------------------------//

// INI ファイルの書き出し
bool __stdcall SaveIniFile(LPCTSTR ininame)
{
    const size_t BUF_SIZE = 64;
    TCHAR buf[BUF_SIZE];

    ::StringCchPrintf(buf, BUF_SIZE, TEXT("%d"), true);
    ::WritePrivateProfileString
    (
        TEXT("settings"), TEXT("enabled"), buf, ininame
    );

    ::StringCchPrintf(buf, BUF_SIZE, TEXT("%u"), g_woeid);
    ::WritePrivateProfileString
    (
        TEXT("settings"), TEXT("woeid"), buf, ininame
    );

    ::StringCchPrintf(buf, BUF_SIZE, TEXT("%d"), g_no_hashtags);
    ::WritePrivateProfileString
    (
        TEXT("settings"), TEXT("no_hashtags"), buf, ininame
    );

    ::StringCchPrintf(buf, BUF_SIZE, TEXT("%u"), g_interval);
    ::WritePrivateProfileString
    (
        TEXT("settings"), TEXT("interval"), buf, ininame
    );

    ::WritePrivateProfileString
    (
        TEXT("settings"), TEXT("user"), g_username, ininame
    );

    return true;
}

//---------------------------------------------------------------------------//

// IniFile.cpp