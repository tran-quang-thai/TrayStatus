#include <windows.h>
#include <shellapi.h>

#define MSG_TRAY (WM_USER + 1)

NOTIFYICONDATAA nid;

HICON ico0;
HICON ico1;
HICON ico2;
HICON ico3;

HICON MakeSmiley(int type) {
    HDC scr = GetDC(NULL);
    HDC mem = CreateCompatibleDC(scr);

    HBITMAP bmp = CreateCompatibleBitmap(scr, 16, 16);
    SelectObject(mem, bmp);

    RECT r = {0,0,16,16};

    // Nền vàng
    HBRUSH bg = CreateSolidBrush(RGB(255, 220, 0));
    FillRect(mem, &r, bg);
    DeleteObject(bg);

    // Mắt
    HBRUSH black = CreateSolidBrush(RGB(0,0,0));

    if (type == 0) {
        // Hai mắt tròn
        Ellipse(mem, 4,4,7,7);
        Ellipse(mem, 9,4,12,7);
    }

    if (type == 1) {
        // Một mắt nháy
        Ellipse(mem, 4,4,7,7);
        MoveToEx(mem, 9,6,NULL);
        LineTo(mem, 12,6);
    }

    if (type == 2) {
        // Hai mắt bình thường
        Ellipse(mem, 4,4,7,7);
        Ellipse(mem, 9,4,12,7);
    }

    if (type == 3) {
        // Mắt kính
        Rectangle(mem, 3,4,8,8);
        Rectangle(mem, 8,4,13,8);
        MoveToEx(mem, 8,6,NULL);
        LineTo(mem, 9,6);
    }

    // Miệng
    if (type == 0) {
        Arc(mem, 3,5,13,15, 3,10, 13,10);
    }

    if (type == 1) {
        Arc(mem, 3,7,13,15, 3,10, 13,10);
    }

    if (type == 2) {
        // Miệng há
        Ellipse(mem, 5,9,11,14);
    }

    if (type == 3) {
        // Miệng mỉm nhẹ
        Arc(mem, 4,8,12,15, 4,11, 12,11);
    }

    DeleteObject(black);

    ICONINFO ii = {0};
    ii.fIcon = TRUE;
    ii.hbmColor = bmp;
    ii.hbmMask = bmp;

    HICON h = CreateIconIndirect(&ii);

    DeleteDC(mem);
    ReleaseDC(NULL, scr);
    DeleteObject(bmp);

    return h;
}

void UpdateIcon() {
    BYTE st = 0;
    if (GetKeyState(VK_CAPITAL) & 1) st |= 1;
    if (GetKeyState(VK_NUMLOCK) & 1) st |= 2;

    HICON h =
        st == 0 ? ico0 :
        st == 1 ? ico1 :
        st == 2 ? ico2 :
                  ico3;

    nid.hIcon = h;
    Shell_NotifyIconA(NIM_MODIFY, &nid);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_CREATE) {
        SetTimer(hWnd, 1, 200, NULL);
    }

    if (msg == WM_TIMER) {
        UpdateIcon();
    }

    if (msg == MSG_TRAY) {
        if (lp == WM_RBUTTONUP) PostQuitMessage(0);
    }

    if (msg == WM_DESTROY) {
        KillTimer(hWnd, 1);
        Shell_NotifyIconA(NIM_DELETE, &nid);

        DestroyIcon(ico0);
        DestroyIcon(ico1);
        DestroyIcon(ico2);
        DestroyIcon(ico3);

        PostQuitMessage(0);
    }

    return DefWindowProcA(hWnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "CNCLS";
    RegisterClassA(&wc);

    HWND hWnd = CreateWindowA("CNCLS", "CNCLS", 0,
                              0,0,0,0,
                              NULL,NULL,hInst,NULL);

    ico0 = MakeSmiley(0);
    ico1 = MakeSmiley(1);
    ico2 = MakeSmiley(2);
    ico3 = MakeSmiley(3);

    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(nid);
    nid.hWnd = hWnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = MSG_TRAY;
    nid.hIcon = ico0;
    lstrcpyA(nid.szTip, "Smiley status");

    Shell_NotifyIconA(NIM_ADD, &nid);

    UpdateIcon();

    MSG msg;
    while (GetMessageA(&msg,NULL,0,0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return 0;
}
