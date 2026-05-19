#include <windows.h>
#include <string>

std::string globalText = "Hello from your GUI window!\nYou can put your weekly calendar here.";

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);

            DrawTextA(hdc, globalText.c_str(), -1, &rect, DT_LEFT | DT_TOP | DT_WORDBREAK);

            EndPaint(hwnd, &ps);
        }
        return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void showGuiWindow(const std::string& text) {
    globalText = text;

    HINSTANCE hInstance = GetModuleHandle(NULL);

    const char CLASS_NAME[] = "MyWindowClass";

    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "Weekly Calendar",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        600, 500,   // <-- WINDOW SIZE HERE
        NULL,
        NULL,
        hInstance,
        NULL
    );

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
