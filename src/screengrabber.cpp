#include <iostream>
#include <windows.h>
#include <fstream>
#include "../include/screengrabber.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


ScreenGrabber::ScreenGrabber() {}
ScreenGrabber::~ScreenGrabber() {
    if (overlay_hwnd) {
        DestroyWindow(overlay_hwnd);
    }
}


bool ScreenGrabber::initialiseGrabber(LPCSTR window_title) {
    hwnd = FindWindowA(NULL, window_title);
    //hwnd = (HWND)0x000806C0; // using handle directly, for testing
    if (!hwnd) {
        std::cout << "Window not found!" << std::endl;
        return false;
    }
    std::cout << "Window successfully found." << std::endl;
    return true;
}

HWND ScreenGrabber::createSneakyOverlay() {
    const char CLASS_NAME[] = "SneakyOverlay";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    RECT target_rect;
    GetWindowRect(hwnd, &target_rect);

    overlay_hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        CLASS_NAME,
        "Overlay",
        WS_POPUP,
        target_rect.left, target_rect.top,
        target_rect.right - target_rect.left,
        target_rect.bottom - target_rect.top,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    SetLayeredWindowAttributes(overlay_hwnd, 0, 255, LWA_ALPHA);
    SetWindowPos(overlay_hwnd, HWND_TOPMOST, target_rect.left, target_rect.top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

    ShowWindow(overlay_hwnd, SW_SHOW);
    return overlay_hwnd;
}


HBITMAP ScreenGrabber::grabSneakyOverlay() {
    RECT target_rect;
    GetWindowRect(hwnd, &target_rect);
    int width = target_rect.right - target_rect.left;
    int height = target_rect.bottom - target_rect.top;

    HDC hdc_screen = GetDC(NULL);
    HDC hdc_mem_dc = CreateCompatibleDC(hdc_screen);

    HBITMAP hbm_capture = CreateCompatibleBitmap(hdc_screen, width, height);
    SelectObject(hdc_mem_dc, hbm_capture);

    // Capture the area behind the overlay
    if (!BitBlt(hdc_mem_dc, 0, 0, width, height, hdc_screen, target_rect.left, target_rect.top, SRCCOPY)) {
        std::cout << "BitBlt operation failed for sneaky overlay." << std::endl;
        DeleteObject(hbm_capture);
        DeleteDC(hdc_mem_dc);
        ReleaseDC(NULL, hdc_screen);
        return nullptr;
    }

    DeleteDC(hdc_mem_dc);
    ReleaseDC(NULL, hdc_screen);

    std::cout << "Screen successfully captured via sneaky overlay. Returning bmp now." << std::endl;
    return hbm_capture;
}

HBITMAP ScreenGrabber::grabScreenViaDesktop() {
    RECT window_rect;
    if (!GetWindowRect(hwnd, &window_rect)) {
        std::cout << "Failed to get window rectangle." << std::endl;
        return nullptr;
    }
    int width = window_rect.right - window_rect.left;
    int height = window_rect.bottom - window_rect.top;

    HDC hdc_window = GetDC(hwnd);
    HDC hdc_mem_dc = CreateCompatibleDC(hdc_window);
    HBITMAP hbm_screen = CreateCompatibleBitmap(hdc_window, width, height);

    if (!hbm_screen) {
        std::cout << "Error creating HBITMAP. Deleting relevant objs." << std::endl;
        DeleteObject(hbm_screen);
        DeleteDC(hdc_mem_dc);
        ReleaseDC(hwnd, hdc_window);
        return nullptr;
    }

    SelectObject(hdc_mem_dc, hbm_screen);

    if (!BitBlt(hdc_mem_dc, 0, 0, width, height, hdc_window, 0, 0, SRCCOPY)) {
        std::cout << "BitBlt operation failed. Deleting relevant objs." << std::endl;
        DeleteObject(hbm_screen);
        DeleteDC(hdc_mem_dc);
        ReleaseDC(hwnd, hdc_window);
        return nullptr;
    }

    DeleteDC(hdc_mem_dc);
    ReleaseDC(hwnd, hdc_window);

    std::cout << "Screen successfully grabbed (via desktop). Returning now." << std::endl;
    return hbm_screen;
}

HBITMAP ScreenGrabber::grabScreenViaParent() {
    HWND parent_hwnd = GetParent(hwnd);
    if (!parent_hwnd) {
        std::cout << "Failed to get parent window." << std::endl;
        return nullptr;
    }

    RECT parent_rect;
    if (!GetWindowRect(parent_hwnd, &parent_rect)) {
        std::cout << "Failed to get parent window rectangle." << std::endl;
        return nullptr;
    }

    RECT child_rect;
    if (!GetWindowRect(hwnd, &child_rect)) {
        std::cout << "Failed to get child window rectangle." << std::endl;
        return nullptr;
    }

    HDC hdc_parent = GetDC(parent_hwnd);
    HDC hdc_mem_dc = CreateCompatibleDC(hdc_parent);

    int width = child_rect.right - child_rect.left;
    int height = child_rect.bottom - child_rect.top;
    HBITMAP hbm_screen = CreateCompatibleBitmap(hdc_parent, width, height);

    if (!hbm_screen) {
        std::cout << "Error creating bitmap." << std::endl;
        DeleteObject(hbm_screen);
        DeleteDC(hdc_mem_dc);
        ReleaseDC(parent_hwnd, hdc_parent);
        return nullptr;
    }

    SelectObject(hdc_mem_dc, hbm_screen);

    if (!BitBlt(hdc_mem_dc, 0, 0, width, height, hdc_parent, child_rect.left, child_rect.top, SRCCOPY)) {
        std::cout << "BitBlt operation failed." << std::endl;
        DeleteObject(hbm_screen);
        DeleteDC(hdc_mem_dc);
        ReleaseDC(parent_hwnd, hdc_parent);
        return nullptr;
    }

    DeleteDC(hdc_mem_dc);
    ReleaseDC(parent_hwnd, hdc_parent);

    std::cout << "Screen successfully grabbed via parent handle. Returning now." << std::endl;
    return hbm_screen;
}

HBITMAP ScreenGrabber::grabScreen() {
    // Process for getting the bitmap of the window, excluding client components like title bar
    RECT client_rect;
    GetClientRect(hwnd, &client_rect);
    POINT top_left = { client_rect.left, client_rect.top };
    ClientToScreen(hwnd, &top_left);
    int width = client_rect.right - client_rect.left;
    int height = client_rect.bottom - client_rect.top;

    HDC hdc_window = GetDC(hwnd);
    HDC hdc_mem_dc = CreateCompatibleDC(hdc_window);
    HBITMAP hbm_screen = CreateCompatibleBitmap(hdc_window, width, height);
    SelectObject(hdc_mem_dc, hbm_screen);
    if (!hbm_screen) {
        DeleteObject(hbm_screen);
        DeleteDC(hdc_mem_dc);
        ReleaseDC(hwnd, hdc_window);
        std::cout << "Error with HBITMAP. hbm_screen object deleted." << std::endl;
        return nullptr;
    }

    BitBlt(hdc_mem_dc, 0, 0, width, height, hdc_window, 0, 0, SRCCOPY);
    if (!BitBlt(hdc_mem_dc, 0, 0, width, height, hdc_window, 0, 0, SRCCOPY)) {
        std::cout << "BitBlt operation failed." << std::endl;
        DeleteObject(hbm_screen);
        DeleteDC(hdc_mem_dc);
        ReleaseDC(hwnd, hdc_window);
        return nullptr;
    }
    // --> the bitmap is now stored in hbm_screen, for processing/saving
    

    // Cleaning up -- NOTE: hbm_screen yet to be deleted; this must be handled by whichever method uses the HBITMAP hbm_screen (e.g., a method which saves the screengrab)
    DeleteDC(hdc_mem_dc);
    ReleaseDC(hwnd, hdc_window);

    std::cout << "Screen successfully grabbed - returning now." << std::endl;
    return hbm_screen;
}

bool ScreenGrabber::saveScreenGrab(HBITMAP &hbm_screen, std::string filename) {
    BITMAP bmp;
    GetObject(hbm_screen, sizeof(bmp), &bmp);

    BITMAPFILEHEADER bmf_header;
    BITMAPINFOHEADER bi;

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = bmp.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 24; // Assuming a 24-bit bitmap
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    DWORD dw_bmp_size = ((bmp.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmp.bmHeight;

    char* lp_bitmap = new char[dw_bmp_size];
    GetDIBits(GetDC(hwnd), hbm_screen, 0, bmp.bmHeight, lp_bitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        delete[] lp_bitmap;
        std::cout << "Error initialising output file. Deleting hbm_screen (call grabScreen again to initialise a new HBITMAP)" << std::endl;
        DeleteObject(hbm_screen);
        return false;
    }

    // Write the file header
    bmf_header.bfType = 0x4D42; // BM
    bmf_header.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dw_bmp_size;
    bmf_header.bfReserved1 = 0;
    bmf_header.bfReserved2 = 0;
    bmf_header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    file.write((char*)&bmf_header, sizeof(BITMAPFILEHEADER));
    file.write((char*)&bi, sizeof(BITMAPINFOHEADER));
    file.write(lp_bitmap, dw_bmp_size);

    delete[] lp_bitmap;
    file.close();
    DeleteObject(hbm_screen);
    return true;
}



