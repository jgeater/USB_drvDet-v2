#include <windows.h>
#include <dbt.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <iostream> // Add this for debugging
#include <commctrl.h> // Include for common controls like fonts



// Helper: Check if any removable drive is present
bool IsUsbPresent() {
    DWORD drives = GetLogicalDrives();
    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        if (drives & (1 << (letter - 'A'))) {
            std::wstring root = std::wstring(1, letter) + L":\\";
            UINT type = GetDriveTypeW(root.c_str());
            // Debug output
            std::wcout << L"Drive " << root << L" type: " << type << std::endl;
            if (type == DRIVE_REMOVABLE) {
                return true;
            }
        }
    }
    return false;
}

// Timer ID
#define TIMER_ID 1

// Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        // Set timer for 3 seconds
        SetTimer(hwnd, TIMER_ID, 3000, NULL);
        break;
    case WM_TIMER:
        if (!IsUsbPresent()) {
            PostQuitMessage(0);
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    if (!IsUsbPresent()) {
        return 0;
    }

    // Register window class
    const wchar_t CLASS_NAME[] = L"UsbNoticeWindow";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    // Desired window size
    int winWidth = 800;
    int winHeight = 240;

    // Get screen size
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);


    // Calculate top-left position to center the window
    int x = (screenWidth - winWidth) / 2;
    int y = (screenHeight - winHeight) / 2;



    // Create window
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        CLASS_NAME,
        L"USB Drive Detected",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        x, y, winWidth, winHeight,
        NULL, NULL, hInstance, NULL);

    if (!hwnd) return 0;

    // Remove the close button from the title bar
    HMENU hMenu = GetSystemMenu(hwnd, FALSE);
    if (hMenu != NULL) {
        RemoveMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
        DrawMenuBar(hwnd);
    }

    // Scale label size and position
    int labelMarginX = static_cast<int>(winWidth * 0.00);
    int labelMarginY = static_cast<int>(winHeight * 0.00);
    // Center label horizontally
    int labelWidth = winWidth - 2 * labelMarginX;
    int labelHeight = static_cast<int>(winHeight * 0.9);
    int labelX = (winWidth - labelWidth) / 2;
    int labelY = labelMarginY; // Keep some top margin

    // Add static label
    HWND hLabel = CreateWindow(
        L"STATIC",
        L"Please remove USB boot media.  The task sequence will continue automatically upon removal",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        labelX, labelY, labelWidth, labelHeight,
        hwnd, NULL, hInstance, NULL);

    // Set a larger font for the label
    HFONT hFont = CreateFont(
        48,                // Height of font (increase for larger size)
        0,                 // Width of font
        0,                 // Escapement angle
        0,                 // Orientation angle
        FW_BOLD,           // Font weight
        FALSE,             // Italic
        FALSE,             // Underline
        FALSE,             // StrikeOut
        DEFAULT_CHARSET,   // Character set
        OUT_DEFAULT_PRECIS,// Output precision
        CLIP_DEFAULT_PRECIS,// Clipping precision
        DEFAULT_QUALITY,   // Quality
        DEFAULT_PITCH | FF_SWISS, // Pitch and family
        L"Segoe UI"        // Font face
    );
    SendMessage(hLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

    // Show window
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}