﻿//---------------------------------------------------------------------------//
//
// MainWindow.h
//  メインウィンドウ
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

#pragma once

//---------------------------------------------------------------------------//
// コントロール識別子
//---------------------------------------------------------------------------//

enum Controls : UINT32
{
    CTRL_BTN_UPDATE = 40001,
    CTRL_LABAEL_INFO,
    CTRL_LABEL_TREND,
};

//---------------------------------------------------------------------------//

// ウィンドウプロシージャ
LRESULT __stdcall MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp);

// イベントハンドラ
LRESULT __stdcall OnCreate(HWND hwnd);
LRESULT __stdcall OnDestroy(HWND hwnd);
LRESULT __stdcall OnSize(HWND hwnd, INT32 w, INT32 h);
LRESULT __stdcall OnEraseBkGnd(HDC hDC);
LRESULT __stdcall OnCommand(HWND hwnd, UINT16 wId, UINT16 nCode);
LRESULT __stdcall OnDwmCompositionChanged(HWND hwnd);
LRESULT __stdcall OnThemeChanged(HWND hwnd);

//---------------------------------------------------------------------------//

// MainWindow.h