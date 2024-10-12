#include <iostream>
#include <windows.h>
#include <fstream>
#include "../include/screengrabber.h"

// This is needed for PrintWindow() within grabPrintWindow()
// This allows it to capture e.g. non-visible window segments
#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002  // Flag for capturing non-visible parts
#endif


ScreenGrabber::ScreenGrabber() {}
ScreenGrabber::~ScreenGrabber() {
    if (overlay_hwnd) {
        DestroyWindow(overlay_hwnd);
    }
}


bool ScreenGrabber::initialiseGrabber(LPCSTR window_title) {
    hwnd = FindWindowA(NULL, window_title);
    if (!hwnd) {
        std::cout << "Window not found!" << std::endl;
        return false;
    }
    std::cout << "Window successfully found." << std::endl;
    return true;
}

HWND ScreenGrabber::getHWND() {
    return hwnd;
}


HBITMAP ScreenGrabber::grabPrintWindow() {

    RECT window_rect, client_rect;
    if (!(GetWindowRect(hwnd, &window_rect) && GetClientRect(hwnd, &client_rect))) {
        std::cout << "Crop Title Bar: Failed to get (WindowRect && ClientRect)..." << std::endl;
    }
    int title_bar_height = (window_rect.bottom - window_rect.top) - (client_rect.bottom - client_rect.top);

    int width = client_rect.right - client_rect.left;
    int height = client_rect.bottom - client_rect.top;

    // Create a compatible DC and bitmap
    HDC hdc_screen = GetDC(NULL);
    HDC hdc_mem_dc = CreateCompatibleDC(hdc_screen);
    HBITMAP hbm_capture = CreateCompatibleBitmap(hdc_screen, width, height);
    SelectObject(hdc_mem_dc, hbm_capture);

    // Use PrintWindow to capture the window's content
    BOOL result = PrintWindow(hwnd, hdc_mem_dc, PW_RENDERFULLCONTENT);
    if (!result) {
        std::cout << "PrintWindow failed." << std::endl;
        DeleteObject(hbm_capture);
        DeleteDC(hdc_mem_dc);
        ReleaseDC(NULL, hdc_screen);
        return nullptr;
    }

    // Cleanup
    DeleteDC(hdc_mem_dc);
    ReleaseDC(NULL, hdc_screen);

    // Cropping out the title bar
    HBITMAP hbm_cropped = cropTitleBar(hbm_capture, title_bar_height);
    // Delete original (non-cropped) bitmap
    DeleteObject(hbm_capture);

    std::cout << "Screen successfully captured using PrintWindow." << std::endl;
    return hbm_cropped;
}


HBITMAP ScreenGrabber::cropTitleBar(HBITMAP &hbm_screen, int title_bar_height) {
    
    BITMAP bmp;
    GetObject(hbm_screen, sizeof(BITMAP), &bmp);
    int new_bmp_height = bmp.bmHeight - title_bar_height;

    HDC hdc_screen = GetDC(NULL);
    HDC hdc_mem_dc = CreateCompatibleDC(hdc_screen);
    HBITMAP hbm_cropped = CreateCompatibleBitmap(hdc_screen, bmp.bmWidth, new_bmp_height);
    SelectObject(hdc_mem_dc, hbm_cropped);

    HDC hdc_capture = CreateCompatibleDC(hdc_screen);
    SelectObject(hdc_capture, hbm_screen);
    BitBlt(hdc_mem_dc, 0, 0, bmp.bmWidth, new_bmp_height, hdc_capture, 0, title_bar_height, SRCCOPY);

    // Cleanup
    DeleteDC(hdc_capture);
    DeleteDC(hdc_mem_dc);
    ReleaseDC(NULL, hdc_screen);

    return hbm_cropped;
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



