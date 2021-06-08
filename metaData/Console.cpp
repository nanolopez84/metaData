#include "Console.h"

Console::Console()
{
    m_hFont = CreateFont(16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Courier new"));

    m_lines.resize(0);
}

void Console::append(const std::string& s)
{
    m_lines.push_back(s);
    if (m_lines.size() > Console::MAX_LINES)
    {
        m_lines.pop_front();
    }
}

void Console::paint(HDC hdc, PAINTSTRUCT ps)
{
    SetDCBrushColor(hdc, COLOR_CONSOLE_BACKGROUND);
    FillRect(hdc, &ps.rcPaint, (HBRUSH) GetStockObject(DC_BRUSH));

    SetBkColor(hdc, COLOR_TEXT_BACKGROUND);
    SetTextColor(hdc, COLOR_TEXT_FOREGROUND);
    SelectObject(hdc, m_hFont);

    int i = 0;
    for(auto it = m_lines.rbegin(); it != m_lines.rend(); ++it)
    {
        std::string line = *it;
        std::wstring stemp = std::wstring(line.begin(), line.end());
        LPCWSTR sw = stemp.c_str();
        TextOut(hdc, 0, CONSOLE_LINE_SPACE * i++, sw, lstrlen(sw));
    }
}