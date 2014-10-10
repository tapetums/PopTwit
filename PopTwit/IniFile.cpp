//---------------------------------------------------------------------------//
//
// IniFile.cpp
//  INI 設定ファイル 操作関数
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>
#include <strsafe.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "IniFile.h"

//---------------------------------------------------------------------------//
// グローバル変数
//---------------------------------------------------------------------------//

extern LPCTSTR APP_NAME;

//---------------------------------------------------------------------------//
// グローバル変数の実体宣言
//---------------------------------------------------------------------------//

bool   g_ask_each_tweet;
size_t g_user_index;
TCHAR  g_username[MAX_ACCOUNT][MAX_PATH];
TCHAR  g_msgstub[MAX_MESSAGE_LEN];

//---------------------------------------------------------------------------//

// INI ファイルの読み込み
bool __stdcall LoadIniFile(LPCTSTR ininame)
{
    const auto enabled = (bool)::GetPrivateProfileIntW
    (
        TEXT("settings"), TEXT("enabled"), (INT)true, ininame
    );
    if ( !enabled )
    {
        return false;
    }

    g_ask_each_tweet = (bool)::GetPrivateProfileIntW
    (
        TEXT("settings"), TEXT("ask_each_tweet"), (INT)true, ininame
    );

    g_user_index = ::GetPrivateProfileIntW
    (
        TEXT("user"), TEXT("index"), 0, ininame
    );

    const size_t BUF_SIZE = 64;
    TCHAR buf[BUF_SIZE];

    for ( size_t index = 0; index < MAX_ACCOUNT; ++index )
    {
#ifdef _NODEFLIB
        ::wnsprintf(buf, BUF_SIZE, TEXT("%02X"), index);
#else
        ::StringCchPrintf(buf, BUF_SIZE, TEXT("%02X"), index);
#endif
        ::GetPrivateProfileString
        (
            TEXT("user"), buf,
            TEXT("-"), g_username[index], MAX_PATH, ininame
        );
    }

    return true;
}

//---------------------------------------------------------------------------//

// INI ファイルの書き込み
bool __stdcall SaveIniFile(LPCTSTR ininame)
{
    const size_t BUF_SIZE = 64;
    TCHAR buf[BUF_SIZE];

#ifdef _NODEFLIB
    ::wnsprintf(buf, BUF_SIZE, TEXT("%d"), true);
#else
    ::StringCchPrintf(buf, BUF_SIZE, TEXT("%d"), true);
#endif
    ::WritePrivateProfileString
    (
        TEXT("settings"), TEXT("enabled"), buf, ininame
    );

#ifdef _NODEFLIB
    ::wnsprintf(buf, BUF_SIZE, TEXT("%d"), g_ask_each_tweet);
#else
    ::StringCchPrintf(buf, BUF_SIZE, TEXT("%d"), g_ask_each_tweet);
#endif
    ::WritePrivateProfileString
    (
        TEXT("settings"), TEXT("ask_each_tweet"), buf, ininame
    );

#ifdef _NODEFLIB
    ::wnsprintf(buf, BUF_SIZE, TEXT("%d"), g_user_index);
#else
    ::StringCchPrintf(buf, BUF_SIZE, TEXT("%d"), g_user_index);
#endif
    ::WritePrivateProfileString
    (
        TEXT("user"), TEXT("index"), buf, ininame
    );

    for ( size_t index = 0; index < MAX_ACCOUNT; ++index )
    {
#ifdef _NODEFLIB
        ::wnsprintf(buf, BUF_SIZE, TEXT("%02X"), index);
#else
        ::StringCchPrintf(buf, BUF_SIZE, TEXT("%02X"), index);
#endif
        ::WritePrivateProfileString
        (
            TEXT("user"), buf, g_username[index], ininame
        );
    }

    return true;
}

//---------------------------------------------------------------------------//

// IniFile.cpp