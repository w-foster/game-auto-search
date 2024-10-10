**AutoSearch Tool for Clash of Clans**

**OOD:**
ScreenGrabber class:
- Handles all functionality relating to capturing the current state of the game
- Does so by using WinAPI to capture a Bitmap of the current game window
- Includes functionality for cropping & saving the .bmp, for use by other objects

BaseProcessor class:
- Handles functionality relating to obtaining & processing information about the current base
- First: uses open source computer vision libraries & APIs to process the current Bitmap, obtaining data such as quantities of available loot
- Second: encapsulates this data, and has methods for processing the resulting 'base' object (e.g. testing it against a set of 'suitability' parameters)

GameInputHandler class:
- Handles functionality for sending inputs to the game, primarily clicks.
- Does so via coordinates received either (i) from the BaseProcessor class, such as clicking an arbitrarily located in-game object found via image processing, or (ii) from member data which maps HUD fields (e.g., 'Next Base' button) to screen coordinates of those regions of interest.

Potential functionality down the line...
Class for Data Analysis:
- Separately from the main task (either after a search session, or via parallelism -- to avoid slowing the main process), store and analyse a stream of base data
- Potentially, this could allow the 'suitability' parameters to be updated automatically, e.g. according to some pre-set time parameter
  - (Set parameters such that bases can be found within an acceptable amount of time)
Current Status:
- Works for CoC running on Windows (11) through the Google Play Games Beta
- The screengrabs are dependent on the native desktop resolution, and the CoC via Play Games seems somewhat blurry; this double-effect means that processing the images successfully will depend on, for example, the text being intelligible
  - A potential solution is 1) having the window maximised, and 2) zooming in somewhat, either i) still in a fixed position, or ii) in multiple positions, changing position if the target in-game objects/text cannot be found.
  - 2)ii) is more complex, but would allow more flexibility for the window's size.
  - Alternatively, the window could be maximised but hidden, as the ScreenGrabber successfully grabs even when the window is off-screen or obscured.