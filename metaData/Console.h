#pragma once

#include <windows.h>

#include <string>
#include <list>

class Console
{
private:
	const COLORREF COLOR_CONSOLE_BACKGROUND = RGB(30, 30, 30);
	const COLORREF COLOR_TEXT_BACKGROUND = COLOR_CONSOLE_BACKGROUND;
	const COLORREF COLOR_TEXT_FOREGROUND = RGB(77, 166, 66);
	const int CONSOLE_LINE_SPACE = 18;
	const size_t MAX_LINES = 36;

	HFONT m_hFont;
	std::list<std::string> m_lines;

public:
	Console();
	void append(const std::string& s);
	void paint(HDC hdc, PAINTSTRUCT ps);
};
