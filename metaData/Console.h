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
	const int CONSOLE_LINE_SPACE = 15;
	const int MAX_LINES = 36;

	std::list<std::string> m_lines;

public:
	Console();
	void append(const std::string& s, HWND hWnd);
	void paint(HDC hdc, PAINTSTRUCT ps);
};
