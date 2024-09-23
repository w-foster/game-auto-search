#include <iostream>
#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class ScreenGrabber {
    HWND hwnd;
    HWND overlay_hwnd;
    
    public:
    // Public attributes
    
    // Constructor and destructor
    ScreenGrabber();
    ~ScreenGrabber();

    // Initialisation methods
    bool initialiseGrabber(LPCSTR window_title);
    HWND createSneakyOverlay();

    // Screen grabbing methods
    HBITMAP grabSneakyOverlay();
    HBITMAP grabPrintWindow(); // CURRENT BEST + WORKING GRAB METHOD
    HBITMAP grabScreenViaDesktop();
    HBITMAP grabScreenViaParent();
    HBITMAP grabScreen();
    bool saveScreenGrab(HBITMAP &hbm_screen, std::string filename);
};