// ConsoleOut.cpp

//---------------------------------------------------------------------------//
//
// デバッグウィンドウへの出力関数
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

#if defined(_DEBUG) || defined(DEBUG)

#include <windows.h>
#include <strsafe.h>

#include "ConsoleOut.h"

//---------------------------------------------------------------------------//

#define CONSOLE_BUFSIZE 1024
#define thread_local __declspec(thread) static

//---------------------------------------------------------------------------//

thread_local SYSTEMTIME st = { };

//---------------------------------------------------------------------------//

void __stdcall console_outA(const char* format, ...)
{
    thread_local char buf[CONSOLE_BUFSIZE];

    // 時刻を文字列に
    ::GetLocalTime(&st);
    ::StringCchPrintfA
    (
        buf, CONSOLE_BUFSIZE,
        "* %02u:%02u:%02u;%03u> ",
        st.wHour, st.wMinute, st.wSecond, st.wMilliseconds
    );

    ::OutputDebugStringA(buf);

    // 引数を文字列に
    va_list al;
    va_start(al, format);
    {
        ::StringCchVPrintfA(buf, CONSOLE_BUFSIZE, format, al);
    }
    va_end(al);

    ::OutputDebugStringA(buf);
    ::OutputDebugStringA("\n");
}

//---------------------------------------------------------------------------//

void __stdcall console_outW(const wchar_t* format, ...)
{
    thread_local wchar_t buf[CONSOLE_BUFSIZE];

    // 時刻を文字列に
    ::GetLocalTime(&st);
    ::StringCchPrintfW
    (
        buf, CONSOLE_BUFSIZE,
        L"* %02u:%02u:%02u;%03u> ",
        st.wHour, st.wMinute, st.wSecond, st.wMilliseconds
    );

    ::OutputDebugStringW(buf);

    // 引数を文字列に
    va_list al;
    va_start(al, format);
    {
        ::StringCchVPrintfW(buf, CONSOLE_BUFSIZE, format, al);
    }
    va_end(al);

    ::OutputDebugStringW(buf);
    ::OutputDebugStringW(L"\n");
}

//---------------------------------------------------------------------------//

#endif // #if defined(_DEBUG) || defined(DEBUG)

//---------------------------------------------------------------------------//

// ConsoleOut.cpp