//---------------------------------------------------------------------------//
//
// Twitter.cpp
//  Twitter 関連の関数
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>

#include "../KeyFile.hpp"
#include "../Transcode.hpp"
#include "ConsoleOut.h"
#include "Twitter.h"
#include "ConsumerKey.h"

//---------------------------------------------------------------------------//

#if _MSC_VER <= 1800
    #if defined(_NODEFLIB)
        #define thread_local static
    #else
        #define thread_local static __declspec(thread)
    #endif
#endif

//---------------------------------------------------------------------------//
// twitcurl ライブラリ を使用
//---------------------------------------------------------------------------//

#include "twitcurl.h"

#ifdef _WIN64
#pragma comment(lib, "lib/x64/libcurl.lib")
#pragma comment(lib, "lib/x64/twitcurl.lib")
#else
#pragma comment(lib, "lib/x86/libcurl.lib")
#pragma comment(lib, "lib/x86/twitcurl.lib")
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
    LPTSTR username; // ユーザー名をダイアログに渡す
    LPTSTR password; // ここにパスワードが返ってくる
};

//---------------------------------------------------------------------------//
// ダイアログ プロシージャ
//---------------------------------------------------------------------------//

// パスワード入力ダイアログのプロシージャ
BOOL CALLBACK PassDlgProc(HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp)
{
    thread_local LPTSTR username = nullptr;
    thread_local LPTSTR password = nullptr;
    thread_local HWND   txt_name = nullptr;
    thread_local HWND   txt_pass = nullptr;

    switch( uMsg )
    {
        case WM_INITDIALOG:
        {
            username = ((DlgData*)lp)->username;
            password = ((DlgData*)lp)->password;
            txt_name = ::GetDlgItem(hwnd, 1002);
            txt_pass = ::GetDlgItem(hwnd, 1004);
            ::SendMessage(txt_name, WM_SETTEXT, 0, (LPARAM)username);

            if ( username[0] == '\0' )
            {
                ::SetFocus(txt_name);
            }
            else
            {
                ::SetFocus(txt_pass);
            }
            return FALSE;
        }
        case WM_COMMAND:
        {
            if ( LOWORD(wp) == IDOK )
            {
                ::GetWindowText(txt_name, username, MAX_PATH);
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
// Twitter 関連の関数
//---------------------------------------------------------------------------//

bool __stdcall ConnectToTwitter
(
    HWND hwnd, LPCTSTR username, twitCurl& twitterObj
)
{
    // Your consumer keys
    std::string myConsumerKey( CONSUMER_KEY );
    std::string myConsumerSecuret( CONSUMER_KEY_SECRET );

    // OAuth flow begins
    twitterObj.getOAuth().setConsumerKey( myConsumerKey );
    twitterObj.getOAuth().setConsumerSecret( myConsumerSecuret );

    // Step 1: Check if we alredy have OAuth access token from a previous run
    std::string myOAuthAccessTokenKey
    (
        (char*)ReadKeyFromFile(username, TEXT("token_key"))
    );
    std::string myOAuthAccessTokenSecret
    (
        (char*)ReadKeyFromFile(username, TEXT("token_secret"))
    );

    if ( myOAuthAccessTokenKey.size() && myOAuthAccessTokenSecret.size() )
    {
        console_outA
        (
            "Using - Key: %s / Secret: %s",
            myOAuthAccessTokenKey.c_str(), myOAuthAccessTokenSecret.c_str()
        );
        twitterObj.getOAuth().setOAuthTokenKey( myOAuthAccessTokenKey );
        twitterObj.getOAuth().setOAuthTokenSecret( myOAuthAccessTokenSecret );

        return true;
    }

    TCHAR buf[MAX_PATH];

    std::string userName;
    std::string passWord;
    std::string replyMsg;

    do
    {
        // Show dialog box for password
        buf[0] = '\0';
        DlgData data = { (LPTSTR)username, buf };
        const auto ret = ::DialogBoxParam
        (
            g_hInst, MAKEINTRESOURCE(1000), hwnd, (DLGPROC)PassDlgProc, (LPARAM)&data
        );
        if ( ret == IDCANCEL )
        {
            console_out(TEXT("%s: Tweet canceled"), APP_NAME);
            return false;
        }
        userName = (char*)toUTF8(username);
        passWord = (char*)toUTF8(buf);

        // Retry to get access token key and secret
        twitterObj.setTwitterUsername( userName );
        twitterObj.setTwitterPassword( passWord );

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
        WriteKeyAsFile
        (
            username, TEXT("token_key"), (char8_t*)myOAuthAccessTokenKey.c_str()
        );
        WriteKeyAsFile
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

        // Delete keys which seemed to be invalid
        DeleteKeyFile(username, TEXT("token_key"));
        DeleteKeyFile(username, TEXT("token_secret"));

    } while ( true );

    return true;
}

//---------------------------------------------------------------------------//

std::string __stdcall GetTrends
(
    HWND hwnd, LPTSTR username, UINT32 woeid, bool no_hashtags
)
{
    twitCurl twitterObj;
    std::string replyMsg;

    // https で接続
    if ( ! ConnectToTwitter(hwnd, username, twitterObj) )
    {
        return std::move( std::string("") );
    }

    // トレンドを取得
    std::string exclude = no_hashtags ? "hashtags" : "";
    if ( twitterObj.trendsGet(woeid, std::move(exclude)) )
    {
        twitterObj.getLastWebResponse( replyMsg );
        console_outA("statusUpdate web response: %s", replyMsg.c_str());
        if ( std::string::npos == replyMsg.find("\"errors\"") )
        {
            console_out(TEXT("Succeeded"));
            return std::move(replyMsg);
        }
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
        console_outA("statusUpdate error: %s", replyMsg.c_str());
    }

    return std::move( std::string("") );
}

//---------------------------------------------------------------------------//

// Twitter.cpp