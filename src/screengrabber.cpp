#include <iostream>
#include <windows.h>
#include <fstream>
#include "../include/screengrabber.h"

ScreenGrabber::ScreenGrabber() {}
ScreenGrabber::~ScreenGrabber() {}

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

HBITMAP ScreenGrabber::grabScreen() {
    // Process for getting the bitmap of the window, excluding client components like title bar
    RECT window_rect;
    GetWindowRect(hwnd, &window_rect);
    int width = window_rect.right - window_rect.left;
    int height = window_rect.bottom - window_rect.top;

    HDC hdc_window = GetDC(hwnd);
    HDC hdc_mem_dc = CreateCompatibleDC(hdc_window);
    HBITMAP hbm_screen = CreateCompatibleBitmap(hdc_window, width, height);
    SelectObject(hdc_mem_dc, hbm_screen);

    BitBlt(hdc_mem_dc, 0, 0, width, height, hdc_window, 0, 0, SRCCOPY);
    // --> the bitmap is now stored in hbm_screen, for processing/saving
    
    if (!hbm_screen) {
        DeleteObject(hbm_screen);
        DeleteDC(hdc_mem_dc);
        ReleaseDC(hwnd, hdc_window);
        std::cout << "Error with HBITMAP. hbm_screen object deleted." << std::endl;
        return nullptr;
    }

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
    GetDIBits(GetDC(0), hbm_screen, 0, bmp.bmHeight, lp_bitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

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

