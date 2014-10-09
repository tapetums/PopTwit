//---------------------------------------------------------------------------//
//
// Twitter.cpp
//  Twitter 関連の関数
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>
#include <strsafe.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "shlobj.h"

#include "ConsoleOut.h"
#include "Twitter.h"

//---------------------------------------------------------------------------//
// twitcurl ライブラリ を使用
//---------------------------------------------------------------------------//

#include "include\twitcurl.h"

#ifdef _WIN64
#pragma comment(lib, "lib\\x64\\libcurl.lib")
#pragma comment(lib, "lib\\x64\\twitcurl.lib")
#else
#pragma comment(lib, "lib\\x86\\libcurl.lib")
#pragma comment(lib, "lib\\x86\\twitcurl.lib")
#endif

//---------------------------------------------------------------------------//
// グローバル変数
//---------------------------------------------------------------------------//

extern LPCTSTR   APP_NAME;
extern HINSTANCE g_hInst;
extern bool      g_ask_each_tweet;

//---------------------------------------------------------------------------//
// 構造体
//---------------------------------------------------------------------------//

// ダイアログに渡すデータ
struct DlgData
{
    LPCTSTR username; // ユーザー名をダイアログに渡す
    LPTSTR  password; // ここにパスワードが返ってくる
};

//---------------------------------------------------------------------------//
// ユーティリティ関数
//---------------------------------------------------------------------------//

#if _MSC_VER <= 1800
    #if defined(_NODEFLIB)
        #define thread_local static
    #else
        #define thread_local static __declspec(thread)
    #endif
#endif

//---------------------------------------------------------------------------//

typedef unsigned char char8_t; // ASCII と UTF-8 を区別するため

//---------------------------------------------------------------------------//

// MBCS -> UTF-8
char8_t* toUTF8(const char* str)
{
    thread_local wchar_t tmp[1024];
    thread_local char8_t buf[1024];

    int len = 0;
    len = ::MultiByteToWideChar
    (
        CP_ACP, 0, str, -1, nullptr, 0
    );
    if ( len < 1 )
    {
        return nullptr;
    }
    ::MultiByteToWideChar
    (
        CP_ACP, 0, str, len, tmp, 1024
    );

    len = ::WideCharToMultiByte
    (
        CP_UTF8, 0, tmp, -1, nullptr, 0, nullptr, nullptr
    );
    if ( len < 1 )
    {
        return nullptr;
    }

    ::WideCharToMultiByte
    (
        CP_UTF8, 0, tmp, -1, (LPSTR)buf, len, nullptr, nullptr
    );

    return buf;
}

//---------------------------------------------------------------------------//

// UTF-16 -> UTF-8
char8_t* toUTF8(const char16_t* str)
{
    thread_local char8_t buf[1024];

    const auto len = ::WideCharToMultiByte
    (
        CP_UTF8, 0, (LPCWSTR)str, -1, nullptr, 0, nullptr, nullptr
    );
    if ( len < 1 )
    {
        return nullptr;
    }

    ::WideCharToMultiByte
    (
        CP_UTF8, 0, (LPCWSTR)str, -1, (LPSTR)buf, len, nullptr, nullptr
    );

    return buf;
}

//---------------------------------------------------------------------------//

// パスワード入力ダイアログのプロシージャ
BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp)
{
    static LPCTSTR username;
    static LPTSTR  password = nullptr;
    static HWND    txt_pass = nullptr;

    switch( uMsg )
    {
        case WM_INITDIALOG:
        {
            username = ((DlgData*)lp)->username;
            password = ((DlgData*)lp)->password;
            txt_pass = ::GetDlgItem(hwnd, 1001);
            ::SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)username);
            ::SetFocus(txt_pass);
            return FALSE;
        }
        case WM_COMMAND:
        {
            if ( LOWORD(wp) == IDOK )
            {
                ::GetWindowText(txt_pass, password, MAX_PATH);
                ::EndDialog(hwnd, IDOK);
                return TRUE;
            }
            else if ( LOWORD(wp) == IDCANCEL )
            {
                ::EndDialog(hwnd, IDCANCEL);
                return TRUE;
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return  FALSE;
}

//---------------------------------------------------------------------------//

char* __stdcall ReadKeyFromFile(LPCTSTR username, LPCTSTR filename)
{
    TCHAR path[MAX_PATH];
    ::SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path);

    TCHAR buf[MAX_PATH];
    ::StringCchPrintf
    (
        buf, MAX_PATH,
        TEXT("%s\\%s\\%s_%s"), path, APP_NAME, username, filename
    );
    console_out(TEXT("ReadKeyFromFile(): %s"), buf);

    const auto hFile = ::CreateFile
    (
        buf, GENERIC_READ, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr
    );
    if ( hFile == nullptr || hFile == INVALID_HANDLE_VALUE )
    {
        return (char*)"";
    }

    DWORD dw;
    char8_t key[MAX_PATH] = { };
    ::ReadFile(hFile, key, MAX_PATH, &dw, nullptr);

    ::CloseHandle(hFile);

    return (dw > 0) ? (char*)key : (char*)"";
}

//---------------------------------------------------------------------------//

bool __stdcall WriteKeyToFile(LPCTSTR username, LPCTSTR filename, const char8_t* key)
{
    TCHAR path[MAX_PATH];
    ::SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path);

    TCHAR buf[MAX_PATH];
    ::StringCchPrintf
    (
        buf, MAX_PATH,
        TEXT("%s\\%s\\%s_%s"), path, APP_NAME, username, filename
    );
    console_out(TEXT("WriteKeyToFile(): %s"), buf);

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
    const auto len = ::lstrlenA((char*)key);
    ::WriteFile(hFile, key, len * sizeof(char8_t), &dw, nullptr);

    ::CloseHandle(hFile);

    return (dw > 0) ? true : false;
}

//---------------------------------------------------------------------------//
// Twitter 関連の関数
//---------------------------------------------------------------------------//

bool __stdcall Tweet
(
    HWND hwnd, size_t index, LPCTSTR username, LPCTSTR message
)
{
    TCHAR buf[MAX_PATH];

    if ( g_ask_each_tweet )
    {
    #ifdef _NODEFLIB
        ::wnsprintf(buf, MAX_PATH, TEXT("@%s から送信しますか？"), username);
    #else
        ::StringCchPrintf(buf, MAX_PATH, TEXT("@%s から送信しますか？"), username);
    #endif

        const auto ret = ::MessageBox
        (
            nullptr, buf, APP_NAME, MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON1
        );
        if ( ret != IDYES )
        {
            return false;
        }
    }

    twitCurl twitterObj;
    std::string replyMsg;

    // ここに自分のコンシューマキーを入力
    std::string myConsumerKey( "" );
    std::string myConsumerSecuret( "" );

    // OAuth flow begins
    twitterObj.getOAuth().setConsumerKey( myConsumerKey );
    twitterObj.getOAuth().setConsumerSecret( myConsumerSecuret );

    // Step 1: Check if we alredy have OAuth access token from a previous run
    std::string myOAuthAccessTokenKey( ReadKeyFromFile(username, TEXT("token_key")) );
    std::string myOAuthAccessTokenSecret( ReadKeyFromFile(username, TEXT("token_secret")) );

    if ( myOAuthAccessTokenKey.size() && myOAuthAccessTokenSecret.size() )
    {
        console_outA
        (
            "Using - Key: %s / Secret: %s",
            myOAuthAccessTokenKey.c_str(), myOAuthAccessTokenSecret.c_str()
        );
        twitterObj.getOAuth().setOAuthTokenKey( myOAuthAccessTokenKey );
        twitterObj.getOAuth().setOAuthTokenSecret( myOAuthAccessTokenSecret );
    }
    else
    {
    #ifdef _UNICODE
        std::string userName( (char*)toUTF8((char16_t*)username) );
    #else
        std::string userName( (char*)account_name) );
    #endif
        std::string passWord;

        // 有効なキーが得られるまで再試行する
        do
        {
            // Step 2: Get request token key and secret
            std::string authUrl;
            twitterObj.oAuthRequestToken( authUrl );

            // Step 3: Get PIN
            // pass auth url to twitCurl and get it via twitCurl PIN handling
            twitterObj.oAuthHandlePIN( authUrl );

            // Step 4: Exchange request token with access token
            twitterObj.oAuthAccessToken();

            // Step 5: save this access token key and secret for future use
            twitterObj.getOAuth().getOAuthTokenKey( myOAuthAccessTokenKey );
            twitterObj.getOAuth().getOAuthTokenSecret( myOAuthAccessTokenSecret );

            // Step 6: Save these keys in a file or wherever
            WriteKeyToFile
            (
                username, TEXT("token_key"), (char8_t*)myOAuthAccessTokenKey.c_str()
            );
            WriteKeyToFile
            (
                username, TEXT("token_secret"), (char8_t*)myOAuthAccessTokenSecret.c_str()
            );

            // Account credentials verification
            if ( twitterObj.accountVerifyCredGet() )
            {
                twitterObj.getLastWebResponse( replyMsg );
                console_outA("accountVerifyCredGet web response: %s", replyMsg.c_str());
                if ( std::string::npos == replyMsg.find("\"errors\"") )
                {
                    console_out(TEXT("Got access token"));
                    break;
                }
            }
            else
            {
                twitterObj.getLastCurlError( replyMsg );
                console_outA("accountVerifyCredGet error: %s", replyMsg.c_str());
            }

            // パスワードの入力を求める
            DlgData data = { username, buf };
            const auto ret = ::DialogBoxParam
            (
                g_hInst, MAKEINTRESOURCE(1000), hwnd, (DLGPROC)DlgProc, (LPARAM)&data
            );
            if ( ret == IDCANCEL )
            {
                console_out(TEXT("CXL"));
                return false;
            }
        #ifdef _UNICODE
            passWord = (char*)toUTF8((char16_t*)buf);
        #else
            passWord = (char*)toUTF8(pass);
        #endif
            console_outA(passWord.c_str());

            // Retry to get access token key and secret
            twitterObj.setTwitterUsername( userName );
            twitterObj.setTwitterPassword( passWord );
            twitterObj.getOAuth().setConsumerKey( myConsumerKey );
            twitterObj.getOAuth().setConsumerSecret( myConsumerSecuret );

        } while ( true );
    }

    // Post a new status message
#ifdef _UNICODE
    std::string msgStr( (char*)toUTF8((char16_t*)message) );
#else
    std::string msgStr( (char*)message) );
#endif
    replyMsg = "";

    if ( twitterObj.statusUpdate( msgStr ) )
    {
        twitterObj.getLastWebResponse( replyMsg );
        console_outA("statusUpdate web response: %s", replyMsg.c_str());
        if ( std::string::npos == replyMsg.find("\"errors\"") )
        {
            console_out(TEXT("Succeeded"));
            return true;
        }
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
        console_outA("statusUpdate error: %s", replyMsg.c_str());
    }

    return false;
}

//---------------------------------------------------------------------------//

bool __stdcall UploadPicture
(
    LPCTSTR filename, LPTSTR uploadedURL, int nMaxLength
)
{
#ifdef _NODEFLIB
    ::wnsprintf(uploadedURL, nMaxLength, TEXT("%s"), filename);
#else
    ::StringCchPrintf(uploadedURL, nMaxLength, TEXT("%s"), filename);
#endif

    return false;
}

//---------------------------------------------------------------------------//

bool __stdcall ShortenURL
(
    LPCTSTR URL, LPTSTR shortenedURL, int nMaxLength
)
{
#ifdef _NODEFLIB
    ::wnsprintf(shortenedURL, nMaxLength, TEXT("%s"), URL);
#else
    ::StringCchPrintf(shortenedURL, nMaxLength, TEXT("%s"), URL);
#endif

    return false;
}

//---------------------------------------------------------------------------//

// Twitter.cpp