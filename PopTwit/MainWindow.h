//---------------------------------------------------------------------------//
//
// MainWindow.hpp
//  メインウィンドウ
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

#pragma once

//---------------------------------------------------------------------------//

// ウィンドウプロシージャ
LRESULT __stdcall MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp);

// イベントハンドラ
LRESULT __stdcall OnCreate(HWND hwnd);
LRESULT __stdcall OnDestroy(HWND hwnd);
LRESULT __stdcall OnSize(HWND hwnd, INT32 w, INT32 h);
LRESULT __stdcall OnPaint(HWND hwnd);
LRESULT __stdcall OnKeyDown(HWND hwnd, INT16 nVirtKey, INT16 lKeyData);
LRESULT __stdcall OnCommand(HWND hwnd, UINT16 wId);
LRESULT __stdcall OnDwmCompositionChanged(HWND hwnd);
LRESULT __stdcall OnThemeChanged(HWND hwnd);

//---------------------------------------------------------------------------//

// MainWindow.hpp