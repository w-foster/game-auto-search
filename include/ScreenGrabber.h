#include <iostream>
#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class ScreenGrabber {
    HWND hwnd;
    HWND overlay_hwnd;
    
public:
    // Constructor and destructor
    ScreenGrabber();
    ~ScreenGrabber();

    // Initialisation methods
    bool initialiseGrabber(LPCSTR window_title);

    // Getter methods
    HWND getHWND();

    // Screen grabbing methods
    HBITMAP grabPrintWindow(); // CURRENT BEST + WORKING GRAB METHOD
    HBITMAP cropTitleBar(HBITMAP &hbm_screen, int title_bar_height);
    bool saveScreenGrab(HBITMAP &hbm_screen, std::string filename);
};