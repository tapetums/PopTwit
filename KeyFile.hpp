//---------------------------------------------------------------------------//
//
// KeyFile.hpp
//  キーファイルの読み書きを行う関数
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

typedef unsigned char char8_t; // MBCS と UTF-8 を区別するため

//---------------------------------------------------------------------------//

char8_t* __stdcall ReadKeyFromFile(LPCTSTR username, LPCTSTR filename);
bool __stdcall WriteKeyAsFile(LPCTSTR username, LPCTSTR filename, const char8_t* key);
bool __stdcall DeleteKeyFile(LPCTSTR username, LPCTSTR filename);

//---------------------------------------------------------------------------//

// KeyFile.hpp