﻿//---------------------------------------------------------------------------//
//
// IniFile.h
//  INI 設定ファイル 操作関数
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
// グローバル変数
//---------------------------------------------------------------------------//

extern UINT32 g_woeid;
extern bool   g_no_hashtags;
extern UINT32 g_interval;
extern TCHAR  g_username[MAX_PATH];

//---------------------------------------------------------------------------//

// INI ファイルの読み書き
bool __stdcall LoadIniFile(LPCTSTR ininame);
bool __stdcall SaveIniFile(LPCTSTR ininame);

//---------------------------------------------------------------------------//

// IniFile.h