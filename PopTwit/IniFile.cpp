//---------------------------------------------------------------------------//
//
// IniFile.h
//  INI 設定ファイル 操作関数
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>
#include <strsafe.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "ConsoleOut.h"
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
    console_out(TEXT("%s: Opening .ini file...: %s"), APP_NAME, ininame);;

    const bool enabled = ::GetPrivateProfileIntW
    (
        TEXT("settings"), TEXT("enabled"), (INT)true, ininame
    );
    if ( !enabled )
    {
        return false;
    }

    g_ask_each_tweet = ::GetPrivateProfileIntW
    (
        TEXT("settings"), TEXT("ask_each_tweet"), (INT)true, ininame
    );
    if ( !enabled )
    {
        return false;
    }

    g_user_index = ::GetPrivateProfileIntW
    (
        TEXT("account"), TEXT("index"), 0, ininame
    );

    TCHAR buf[64];
    for ( size_t index = 1; index < MAX_ACCOUNT; ++index )
    {
#ifdef _NODEFLIB
        ::wnsprintf(buf, 64, TEXT("%02X"), index);
#else
        ::StringCchPrintf(buf, 64, TEXT("%02X"), index);
#endif
        ::GetPrivateProfileString
        (
            TEXT("account"), buf,
            TEXT(""), g_username[index], MAX_PATH, ininame
        );
    }

#ifdef _NODEFLIB
    ::wnsprintf(g_account[0], MAX_PATH, TEXT("(account)"));
#else
    ::StringCchPrintf(g_username[0], MAX_PATH, TEXT("(account)"));
#endif

    return true;
}

//---------------------------------------------------------------------------//

// INI ファイルの書き込み
bool __stdcall SaveIniFile(LPCTSTR ininame)
{
    console_out(TEXT("%s: Saving .ini file...: %s"), APP_NAME, ininame);;

    TCHAR buf[64];

#ifdef _NODEFLIB
    ::wnsprintf(buf, 64, TEXT("%d"), true);
#else
    ::StringCchPrintf(buf, 64, TEXT("%d"), true);
#endif
    ::WritePrivateProfileString
    (
        TEXT("settings"), TEXT("enabled"), buf, ininame
    );

#ifdef _NODEFLIB
    ::wnsprintf(buf, 64, TEXT("%d"), g_ask_each_tweet);
#else
    ::StringCchPrintf(buf, 64, TEXT("%d"), g_ask_each_tweet);
#endif
    ::WritePrivateProfileString
    (
        TEXT("settings"), TEXT("ask_each_tweet"), buf, ininame
    );

#ifdef _NODEFLIB
    ::wnsprintf(buf, 64, TEXT("%d"), g_user_index);
#else
    ::StringCchPrintf(buf, 64, TEXT("%d"), g_user_index);
#endif
    ::WritePrivateProfileString
    (
        TEXT("account"), TEXT("index"), buf, ininame
    );

    for ( size_t index = 1; index < MAX_ACCOUNT; ++index )
    {
#ifdef _NODEFLIB
        ::wnsprintf(buf, 64, TEXT("%02X"), index);
#else
        ::StringCchPrintf(buf, 64, TEXT("%02X"), index);
#endif
        ::WritePrivateProfileString
        (
            TEXT("account"), buf, g_username[index], ininame
        );
    }

    console_out(TEXT("%s: Saved .ini file"), APP_NAME, ininame);;

    return true;
}

//---------------------------------------------------------------------------//

// IniFile.h