#include "Console.h"

Console::Console()
{
    m_lines.resize(0);
}

void Console::append(const std::string& s, HWND hWnd)
{
    m_lines.push_back(s);
    if (m_lines.size() > Console::MAX_LINES)
    {
        m_lines.pop_front();
    }

    // Force repaint
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
}

void Console::paint(HDC hdc, PAINTSTRUCT ps)
{
    SetDCBrushColor(hdc, COLOR_CONSOLE_BACKGROUND);
    FillRect(hdc, &ps.rcPaint, (HBRUSH) GetStockObject(DC_BRUSH));

    SetBkColor(hdc, COLOR_TEXT_BACKGROUND);
    SetTextColor(hdc, COLOR_TEXT_FOREGROUND);

    int i = 0;
    for(auto it = m_lines.rbegin(); it != m_lines.rend(); ++it)
    {
        std::string line = *it;
        std::wstring stemp = std::wstring(line.begin(), line.end());
        LPCWSTR sw = stemp.c_str();
        TextOut(hdc, 0, CONSOLE_LINE_SPACE * i++, sw, lstrlen(sw));
    }
}