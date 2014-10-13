//---------------------------------------------------------------------------//
//
// KeyFile.hpp
//  キーファイルの読み書きを行う関数
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

#include "shlobj.h"

#include "KeyFile.hpp"

//---------------------------------------------------------------------------//

#if _MSC_VER <= 1800
    #if defined(_NODEFLIB)
        #define thread_local static
    #else
        #define thread_local static __declspec(thread)
    #endif
#endif

//---------------------------------------------------------------------------//
// グローバル変数
//---------------------------------------------------------------------------//

extern LPCTSTR   APP_NAME;
extern HINSTANCE g_hInst;

//---------------------------------------------------------------------------//

typedef unsigned char char8_t; // MBCS と UTF-8 を区別するため

//---------------------------------------------------------------------------//

char8_t* __stdcall ReadKeyFromFile(LPCTSTR username, LPCTSTR filename)
{
    thread_local char8_t key[MAX_PATH];
    memset(key, 0, MAX_PATH * sizeof(char8_t));

    TCHAR path[MAX_PATH];
    ::SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path);

    TCHAR buf[MAX_PATH];
    ::StringCchPrintf
    (
        buf, MAX_PATH,
        TEXT("%s\\%s\\%s_%s"), path, APP_NAME, username, filename
    );

    const auto hFile = ::CreateFile
    (
        buf, GENERIC_READ, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr
    );
    if ( hFile == nullptr || hFile == INVALID_HANDLE_VALUE )
    {
        return (char8_t*)"";
    }

    DWORD dw;
    ::ReadFile(hFile, key, MAX_PATH, &dw, nullptr);

    ::CloseHandle(hFile);

    return (dw > 0) ? (char8_t*)key : (char8_t*)"";
}

//---------------------------------------------------------------------------//

bool __stdcall WriteKeyAsFile(LPCTSTR username, LPCTSTR filename, const char8_t* key)
{
    TCHAR path[MAX_PATH];
    ::SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path);

    TCHAR buf[MAX_PATH];
    ::StringCchPrintf
    (
        buf, MAX_PATH,
        TEXT("%s\\%s"), path, APP_NAME
    );
    const auto ret = ::CreateDirectory(buf, nullptr);
    ::StringCchPrintf
    (
        buf, MAX_PATH,
        TEXT("%s\\%s\\%s_%s"), path, APP_NAME, username, filename
    );

    const auto hFile = ::CreateFile
    (
        buf, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr
    );
    if ( hFile == nullptr || hFile == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    DWORD dw;
    const auto len = ::lstrlenA((char*)key); // NULL文字は書き込まないので +1 しない
    ::WriteFile(hFile, key, len * sizeof(char8_t), &dw, nullptr);
    ::SetEndOfFile(hFile);

    ::CloseHandle(hFile);

    return (dw > 0) ? true : false;
}

//---------------------------------------------------------------------------//

bool __stdcall DeleteKeyFile(LPCTSTR username, LPCTSTR filename)
{
    TCHAR path[MAX_PATH];
    ::SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path);

    TCHAR buf[MAX_PATH];
    ::StringCchPrintf
    (
        buf, MAX_PATH,
        TEXT("%s\\%s\\%s_%s"), path, APP_NAME, username, filename
    );

    return  ::DeleteFile(buf) ? true : false;
}

//---------------------------------------------------------------------------//

// KeyFile.hpp