#include <windows.h>
#include <shellapi.h>

NOTIFYICONDATA nid;

HICON MakeIcon(const char* text) {
    HDC hdc = GetDC(NULL);
    HDC mem = CreateCompatibleDC(hdc);

    HBITMAP bmp = CreateCompatibleBitmap(hdc, 16, 16);
    SelectObject(mem, bmp);

    RECT rc = {0, 0, 16, 16};

    HBRUSH bg = CreateSolidBrush(RGB(0,0,0));
    FillRect(mem, &rc, bg);
    DeleteObject(bg);

    SetBkMode(mem, TRANSPARENT);	
    SetTextColor(mem, RGB(255,255,255));

    DrawTextA(mem, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    ICONINFO ii;
    ii.fIcon = TRUE;
    ii.xHotspot = 0;
    ii.yHotspot = 0;
    ii.hbmMask = bmp;
    ii.hbmColor = bmp;

    HICON hIcon = CreateIconIndirect(&ii);

    DeleteDC(mem);
    ReleaseDC(NULL, hdc);

    return hIcon;
}

void UpdateIcon() {
    bool c = GetKeyState(VK_CAPITAL) & 1;
    bool n = GetKeyState(VK_NUMLOCK) & 1;

    const char* txt;

    if (c && n) txt = "CN";
    else if (c) txt = "C";
    else if (n) txt = "N";
    else txt = "HI";

    HICON ic = MakeIcon(txt);
    nid.hIcon = ic;
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_TIMER) UpdateIcon();
    if (m == WM_USER + 1 && l == WM_RBUTTONUP) PostQuitMessage(0);
    return DefWindowProc(h, m, w, l);
}

int WINAPI WinMain(HINSTANCE h, HINSTANCE, LPSTR, int) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = h;
    wc.lpszClassName = "CNICON";
    RegisterClass(&wc);

    HWND win = CreateWindow("CNICON", "", 0, 0, 0, 0, 0, 0, 0, h, 0);

    nid.cbSize = sizeof(nid);
    nid.hWnd = win;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 1;
    lstrcpy(nid.szTip, "Caps Num Status");

    nid.hIcon = MakeIcon(":)");
    Shell_NotifyIcon(NIM_ADD, &nid);

    SetTimer(win, 1, 200, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);

    Shell_NotifyIcon(NIM_DELETE, &nid);
    return 0;
}