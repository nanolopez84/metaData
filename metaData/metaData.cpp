// metaData.cpp: Defines the entry point for the application

#include "framework.h"
#include "metaData.h"

#include <shellapi.h>

#include <sstream>
#include <map>
#include <vector>
#include <memory>
#include <list>
#include <fstream>
#include <filesystem>

#include "Memory.h"

#define MAX_LOADSTRING 100

// Global Variables
HINSTANCE   hInst;                          // Current instance
HWND        hWnd;                           // Main window handler
WCHAR       szTitle[MAX_LOADSTRING];        // The title bar text
WCHAR       szWindowClass[MAX_LOADSTRING];  // The main window class name

Console                 g_console;          // Console text
Context                 g_state;            // Program state
std::unique_ptr<Memory> g_memory;           // Game target memory access

// Forward declarations of functions included in this code module
void                CreateKeyMappingFile();
LRESULT CALLBACK    HookCallback(int code, WPARAM wParam, LPARAM lParam);
void                InitConfiguration(LPWSTR lpCmdLine);
HWND                InitInstance(HINSTANCE, int);
ATOM                MyRegisterClass(HINSTANCE hInstance);
void                Usage();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_METADATA, szWindowClass, MAX_LOADSTRING);

    InitConfiguration(lpCmdLine);

    MyRegisterClass(hInstance);

    // Perform application initialization
    hWnd = InitInstance(hInstance, nCmdShow);
    if (!hWnd)
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_METADATA));

    HHOOK hookId = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, hInstance, 0);

    // Main message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    UnhookWindowsHookEx(hookId);

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_METADATA));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_METADATA);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_MINIMIZEBOX | WS_SYSMENU,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return NULL;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return hWnd;
}

//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            g_console.paint(hdc, ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        Exit(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK HookCallback(int code, WPARAM wParam, LPARAM lParam)
{
    if (code >= 0 && wParam == WM_KEYDOWN)
    {
#if 0
        std::stringstream ss;
        ss << "vkCode: " << keyStruct->vkCode << std::endl;
        OutputDebugStringA(ss.str().c_str());
#endif

        KBDLLHOOKSTRUCT* keyStruct = (KBDLLHOOKSTRUCT*)lParam;
        g_state.update(keyStruct->vkCode);
    }
    return CallNextHookEx(NULL, code, wParam, lParam);
}

void InitConfiguration(LPWSTR lpCmdLine)
{
    LPWSTR* szArglist;
    int nArgs;
    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if (NULL == szArglist)
    {
        g_state.setState(Context::STATES::ERR);
        g_state.setErrorMessage("Error parsing arguments");
        return;
    }
    else if (nArgs != 2)
    {
        Usage();
    }
    else if (std::wstring(L"default").compare(szArglist[1]) == 0)
    {
        g_state.setState(Context::STATES::ERR);
        CreateKeyMappingFile();
    }
    else
    {
        g_memory = MemoryFactory::Get().createMemory(szArglist[1]);
        if (g_memory)
        {
            g_state.setState(Context::STATES::WORKING);
        }
    }

    LocalFree(szArglist);
}

void CreateKeyMappingFile()
{
    int width = 60;

    std::ofstream fout(KEY_MAP_FILE_NAME);

    if (!fout.bad())
    {
        fout << std::left << std::setw(width) << "NINJA_INSTANT_CAST_OFF" << 'X' << std::endl;
        fout << std::left << std::setw(width) << "NINJA_INSTANT_CAST_ON" << 'C' << std::endl;
        fout << std::left << std::setw(width) << "NINJA_INFINITE_INVENTORY_OFF" << 'Z' << std::endl;
        fout << std::left << std::setw(width) << "NINJA_INFINITE_INVENTORY_ON" << 'V' << std::endl;
    }

    g_console.append("Press ESC to exit");
    g_console.append("");

    if (!fout.bad())
    {
        g_console.append("Key mapping file created");
    }
    else
    {
        std::stringstream ss;
        ss << "Can not create key mapping file: " << KEY_MAP_FILE_NAME;
        g_console.append(ss.str());
    }
}

void Exit(int nExitCode)
{
    PostQuitMessage(nExitCode);
}

void InvalidateScreen()
{
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
}

void Usage()
{
    g_state.setState(Context::STATES::ERR);

    /*
    * The Console keeps showing lines in reverse so a buffering and reordering
    * is needed to display a multi-line message
    */

    std::stringstream ss;
    std::vector<std::string> lines;

    lines.push_back("Usage:");

    ss << PROGRAM_NAME << " <target>";
    lines.push_back(ss.str());

    lines.push_back("");
    lines.push_back("Example:");

    ss.str("");
    ss.clear();
    ss << PROGRAM_NAME << " ninja";
    lines.push_back(ss.str());

    lines.push_back("");
    lines.push_back("Available targets:");
    lines.push_back("ninja                Mark of the Ninja");
    lines.push_back("re2                  Resident Evil 2 (Remake)");

    lines.push_back("");
    lines.push_back("Reset keyboard mapping:");

    ss.str("");
    ss.clear();
    ss << PROGRAM_NAME << " default";
    lines.push_back(ss.str());

    lines.push_back("");
    lines.push_back("Press ESC to exit");

    for (auto it = lines.rbegin(); it != lines.rend(); ++it)
    {
        g_console.append(*it);
    }
}