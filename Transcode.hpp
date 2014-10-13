//---------------------------------------------------------------------------//
//
// Transcode.hpp
//  文字列エンコード変換関数
//   Copyright (C) 2014 tapetums
//
//---------------------------------------------------------------------------//

#include <cstdint>

//---------------------------------------------------------------------------//

typedef unsigned char char8_t; // MBCS と UTF-8 を区別するため

//---------------------------------------------------------------------------//

// UTF-8 -> MBCS
char* toMBCS(const char8_t* str_u8);

// MBCS -> UTF-8
char8_t* toUTF8(const char* str_mbcs);

// UTF-16 -> UTF-8
char8_t* toUTF8(const wchar_t* str_u16);

// MBCS -> UTF-16
char16_t* toUTF16(const char* str_mbcs);

// UTF-8 -> UTF-16
char16_t* toUTF16(const char8_t* str_u8);

//---------------------------------------------------------------------------//

// Transcode.hpp