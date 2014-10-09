//---------------------------------------------------------------------------//
//
// MainWindow.Utl.hpp
//  ユーティリティ関数
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

#pragma once

//---------------------------------------------------------------------------//
// ユーティリティ関数
//---------------------------------------------------------------------------//

HFONT __stdcall MakeFont(INT32 font_size, LPCTSTR font_name)
{
    const auto hFont = ::CreateFont
    (
        font_size, 0, 0, 0,
        FW_REGULAR, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
        font_name
    );

    return hFont;
}

//---------------------------------------------------------------------------//

bool __stdcall DeleteFont(HFONT hFont)
{
    if ( hFont == nullptr )
    {
        return false;
    }

    ::DeleteObject(hFont);
    hFont = nullptr;

    return true;
}

//---------------------------------------------------------------------------//

void __stdcall ClearMessagestub(LPTSTR msgstub)
{
    memset(msgstub, 0, MAX_MESSAGE_LEN * sizeof(TCHAR));
}

//---------------------------------------------------------------------------//

bool __stdcall LoadMessageStub(LPTSTR msgstub)
{
    TCHAR path[MAX_PATH];
    ::SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path);

    TCHAR buf[MAX_PATH];
    ::StringCchPrintf(buf, MAX_PATH, TEXT("%s\\%s\\stub"), path, APP_NAME);

    const auto hFile = ::CreateFile
    (
        buf, GENERIC_READ, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr
    );
    if ( hFile == nullptr || hFile == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    DWORD dw;
    ::ReadFile(hFile, msgstub, MAX_PATH, &dw, nullptr);

    ::CloseHandle(hFile);

    return (dw > 0) ? true : false;
}

//---------------------------------------------------------------------------//

bool __stdcall SaveMessageStub(LPCTSTR msgstub)
{
    TCHAR path[MAX_PATH];
    ::SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path);

    TCHAR buf[MAX_PATH];
    ::StringCchPrintf(buf, MAX_PATH, TEXT("%s\\%s\\stub"), path, APP_NAME);

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
    ::WriteFile(hFile, msgstub, MAX_MESSAGE_LEN * sizeof(TCHAR), &dw, nullptr);

    ::CloseHandle(hFile);

    return (dw > 0) ? true : false;
}

//---------------------------------------------------------------------------//

HRESULT __stdcall OpenDialog(LPTSTR filename, size_t nMaxLength)
{
    if ( filename == nullptr )
    {
        return E_POINTER;
    }
    filename[0] = '\0';

    HRESULT hr = S_FALSE;

    IFileOpenDialog* dialog = nullptr;
    IShellItem*      item   = nullptr;

    hr = ::CoCreateInstance
    (
        CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
        IID_IFileOpenDialog, (void**)&dialog
    );
    if ( FAILED(hr) )
    {
        goto END;
    }

    hr = dialog->Show(nullptr);
    if ( FAILED(hr) )
    {
        goto END;
    }

    hr = dialog->GetResult(&item);
    if ( FAILED(hr) )
    {
        goto END;
    }

    LPWSTR path = nullptr;
    hr = item->GetDisplayName(SIGDN_FILESYSPATH, &path);
    if ( FAILED(hr) )
    {
        goto END;
    }

    ::StringCchCopy(filename, nMaxLength, path);
    ::CoTaskMemFree(path);

END:
    if ( item )
    {
        item->Release();
        item = nullptr;
    }
    if ( dialog )
    {
        dialog->Release();
        dialog = nullptr;
    }

    return hr;
}

//---------------------------------------------------------------------------//
// サブクラス プロシージャ
//---------------------------------------------------------------------------//

LRESULT __stdcall ComboBoxProc
(
    HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData
)
{
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

LRESULT __stdcall TextBoxProc
(
    HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData
)
{
    if ( uMsg == WM_KEYDOWN )
    {
        // TODO: Ctrl+Enter でも改行が入力されてしまうのを 修正する
        Wnd::Refresh(hwnd);

        const auto nVirtKey = (INT16)wp;
        if
        (
            nVirtKey == VK_ESCAPE ||
            nVirtKey == VK_RETURN && (0x80 & ::GetKeyState(VK_CONTROL)) ||
            nVirtKey == VK_UP     && (0x80 & ::GetKeyState(VK_CONTROL)) ||
            nVirtKey == VK_DOWN   && (0x80 & ::GetKeyState(VK_CONTROL))
        )
        {
            return OnKeyDown((HWND)dwRefData, (INT16)wp, (INT16)lp);
        }
    }
    else if ( uMsg == WM_PAINT )
    {
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
            ::FillRect(hMemDC, prc, (HBRUSH)::GetStockObject(WHITE_BRUSH));
            ::SendMessage(hwnd, WM_PRINTCLIENT, (WPARAM)hMemDC, PRF_CLIENT);
            ::BufferedPaintSetAlpha(pb, prc, 255);
            ::EndBufferedPaint(pb, TRUE);
        }

        ::EndPaint(hwnd, &ps);

        return 0;
    }

    return ::DefSubclassProc(hwnd, uMsg, wp, lp);
};

//---------------------------------------------------------------------------//

LRESULT __stdcall ButtonProc
(
    HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData
)
{
    if ( uMsg == WM_LBUTTONUP )
    {
        const auto hwnd_parent = (HWND)dwRefData;
        OnCommand(hwnd_parent, (UINT16)uIdSubclass);

        return ::DefSubclassProc(hwnd, uMsg, wp, lp);
    }
    else if ( uMsg != WM_PAINT )
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
