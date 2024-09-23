#include <iostream>
#include <windows.h>

class ScreenGrabber {
    HWND hwnd;
    
    public:
    // Public attributes

    // Constructor and destructor
    ScreenGrabber();
    ~ScreenGrabber();

    bool initialiseGrabber(LPCSTR window_title);
    HBITMAP grabScreenViaDesktop();
    HBITMAP grabScreen();
    bool saveScreenGrab(HBITMAP &hbm_screen, std::string filename);
};