//---------------------------------------------------------------------------//
//
// Font.hpp
//  フォントの作成および破棄
//   Copyright (C) 2014 tapetums
//
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

// Font.hpp