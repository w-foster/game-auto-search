**AutoSearch Tool for Clash of Clans**

Current Status:
- Implemented a ScreenGrabber class for grabbing a full bitmap of the CoC window
  - This class has a method which successfully tracks the window regardless of location on the desktop, and successfully ignores any overlapping windows/tabs
- Works for CoC running on Windows through the Google Play Games Beta
- The screengrabs are dependent on the native desktop resolution, and the CoC via Play Games seems somewhat blurry; this double-effect means that processing the images successfully will depend on, for example, the text being intelligible
  - A potential solution is 1) having the window maximised, and 2) zooming in somewhat, either i) still in a fixed position, or ii) in multiple positions, changing position if the target in-game objects/text cannot be found.
  - 2)ii) is more complex, but would allow more flexibility for the window's size.