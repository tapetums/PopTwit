//---------------------------------------------------------------------------//
//
// Transcode.hpp
//  文字列エンコード変換関数
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>

#include "Transcode.hpp"

//---------------------------------------------------------------------------//

#if _MSC_VER <= 1800
    #if defined(_NODEFLIB)
        #define thread_local static
    #else
        #define thread_local static __declspec(thread)
    #endif
#endif

//---------------------------------------------------------------------------//

typedef unsigned char char8_t; // MBCS と UTF-8 を区別するため

//---------------------------------------------------------------------------//
//  文字列エンコード変換関数
//---------------------------------------------------------------------------//

// UTF-8 -> MBCS
char* toMBCS(const char8_t* str_u8)
{
    thread_local char16_t str_u16 [1024];
    thread_local char     str_mbcs[1024];

    int len = 0;
    len = ::MultiByteToWideChar
    (
        CP_UTF8, 0, (LPCCH)str_u8, -1, nullptr, 0
    );
    if ( len < 1 || 1024 < len )
    {
        return (char*)"";
    }

    ::MultiByteToWideChar
    (
        CP_UTF8, 0, (LPCCH)str_u8, -1, (LPWSTR)str_u16, len
    );

    len = ::WideCharToMultiByte
    (
        CP_ACP, 0, (LPCWSTR)str_u16, -1, nullptr, 0, nullptr, nullptr
    );
    if ( len < 1 || 1024 < len )
    {
        return (char*)"";
    }

    ::WideCharToMultiByte
    (
        CP_ACP, 0, (LPCWSTR)str_u16, -1, str_mbcs, len, nullptr, nullptr
    );

    return str_mbcs;
}

//---------------------------------------------------------------------------//

// MBCS -> UTF-8
char8_t* toUTF8(const char* str_mbcs)
{
    thread_local char16_t str_u16[1024];
    thread_local char8_t  str_u8 [1024];

    int len = 0;
    len = ::MultiByteToWideChar
    (
        CP_ACP, MB_PRECOMPOSED, str_mbcs, -1, nullptr, 0
    );
    if ( len < 1 || 1024 < len )
    {
        return (char8_t*)"";
    }

    ::MultiByteToWideChar
    (
        CP_ACP, MB_PRECOMPOSED, str_mbcs, -1, (LPWSTR)str_u16, len
    );

    len = ::WideCharToMultiByte
    (
        CP_UTF8, 0, (LPCWSTR)str_u16, -1, nullptr, 0, nullptr, nullptr
    );
    if ( len < 1 || 1024 < len )
    {
        return (char8_t*)"";
    }

    ::WideCharToMultiByte
    (
        CP_UTF8, 0, (LPCWSTR)str_u16, -1, (LPSTR)str_u8, len, nullptr, nullptr
    );

    return str_u8;
}

//---------------------------------------------------------------------------//

// UTF-16 -> UTF-8
char8_t* toUTF8(const wchar_t* str_u16)
{
    thread_local char8_t str_u8[1024];

    const auto len = ::WideCharToMultiByte
    (
        CP_UTF8, 0, str_u16, -1, nullptr, 0, nullptr, nullptr
    );
    if ( len < 1 || 1024 < len )
    {
        return (char8_t*)"";
    }

    ::WideCharToMultiByte
    (
        CP_UTF8, 0, str_u16, -1, (LPSTR)str_u8, len, nullptr, nullptr
    );

    return str_u8;
}

//---------------------------------------------------------------------------//

// MBCS -> UTF-16
char16_t* toUTF16(const char* str_mbcs)
{
    thread_local char16_t str_u16 [1024];

    const auto len = ::MultiByteToWideChar
    (
        CP_ACP, MB_PRECOMPOSED, str_mbcs, -1, nullptr, 0
    );
    if ( len < 1 || 1024 < len )
    {
        return (char16_t*)"";
    }

    ::MultiByteToWideChar
    (
        CP_ACP, MB_PRECOMPOSED, str_mbcs, -1, (LPWSTR)str_u16, len
    );

    return str_u16;
}

//---------------------------------------------------------------------------//

// UTF-8 -> UTF-16
char16_t* toUTF16(const char8_t* str_u8)
{
    thread_local char16_t str_u16[1024];

    const auto len = ::MultiByteToWideChar
    (
        CP_UTF8, 0, (LPCCH)str_u8, -1, nullptr, 0
    );
    if ( len < 1 || 1024 < len )
    {
        return (char16_t*)"";
    }

    ::MultiByteToWideChar
    (
        CP_UTF8, 0, (LPCCH)str_u8, -1, (LPWSTR)str_u16, len
    );

    return str_u16;
}

//---------------------------------------------------------------------------//

// Transcode.hpp